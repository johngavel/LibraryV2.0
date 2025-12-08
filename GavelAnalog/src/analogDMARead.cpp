#include "analogDMARead.h"

#include <Arduino.h>
#include <hardware/adc.h>

// set this to determine sample rate of 1 channel
// Divide by the number of channels
// 0/96  = 500,000 Hz
// 480   = 100,000 Hz
// 960   = 50,000 Hz
// 9600  = 5,000 Hz
#define CLOCK_ADC 48000000                                                     // The ADC Clock is running at 48Mhz
#define CLOCK_DIV 480                                                          // Number of cycles to take 1 sample
#define NUMBER_OF_SAMPLES_PER_SECOND ((double) CLOCK_ADC / (double) CLOCK_DIV) // Example: 48Mhz / 480 = 100K Samples
#define SAMPLE_TIME_US (1000000.0 / (double) NUMBER_OF_SAMPLES_PER_SECOND)     // Micro Seconds for one sample
#define FULL_SAMPLE_TIME (SAMPLE_TIME_US * CAPTURE_DEPTH)

void AnalogDmaRead::setupDMA() {
  // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
  for (int i = 0; i < CAPTURE_PINS; i++) adc_gpio_init(26 + i);

  adc_init();
  adc_set_round_robin(ROUND_ROBIN_MASK);
  adc_select_input(0);
  adc_fifo_setup(true,  // Write each completed conversion to the sample FIFO
                 true,  // Enable DMA data request (DREQ)
                 1,     // DREQ (and IRQ) asserted when at least 1 sample present
                 false, // We won't see the ERR bit because of 8 bit reads; disable.
                 false  // Shift each sample to 8 bits when pushing to FIFO
  );

  // Divisor of 0 -> full speed. Free-running capture with the divider is
  // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
  // cycles (div not necessarily an integer). Each conversion takes 96
  // cycles, so in general you want a divider of 0 (hold down the button
  // continuously) or > 95 (take samples less frequently than 96 cycle
  // intervals). This is all timed by the 48 MHz ADC clock.
  adc_set_clkdiv(CLOCK_DIV);
  analogReadResolution(12);

  // Set up the DMA to start transferring data as soon as it appears in FIFO
  dma_chan = dma_claim_unused_channel(true);
  cfg = dma_channel_get_default_config(dma_chan);

  // Reading from constant address, writing to incrementing byte addresses
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);

  // Pace transfers based on availability of ADC samples
  channel_config_set_dreq(&cfg, DREQ_ADC);
}

void AnalogDmaRead::gatherSamples() {
  downTime.stop();
  totalTime.start();
  startSample();
  analyseBuffer();
  finishSample();
  totalTime.stop();
  downTime.start();
}

ADC_BUFFER* AnalogDmaRead::getData() {
  return &buffer[lastBuffer];
}

void AnalogDmaRead::startSample() {
  adc_run(false);
  adc_fifo_drain();

  lastBuffer = currentBuffer;
  dma_channel_configure(dma_chan, &cfg,
                        buffer[currentBuffer].capture_buf, // dst
                        &adc_hw->fifo,                     // src
                        CAPTURE_DEPTH,                     // transfer count
                        true                               // start immediately
  );
  adc_run(true);
  buffer[currentBuffer].timestamp = micros();
  currentBuffer = (currentBuffer + 1) % CAPTURE_PINS;
}

void AnalogDmaRead::analyseBuffer() {
  analysisTime.start();
  if (transfer != nullptr) transfer->processCapture(&buffer[currentBuffer]);
  memset(buffer[currentBuffer].capture_buf, 0, CAPTURE_BUFFER_SIZE);
  analysisTime.stop();
}

void AnalogDmaRead::finishSample() {
  dma_channel_wait_for_finish_blocking(dma_chan);
  dma_channel_cleanup(dma_chan);
  dma_channel_unclaim(dma_chan);
}

bool AnalogDmaRead::setupTask(OutputInterface* __terminal) {
  setRefreshMicro(FULL_SAMPLE_TIME);
  currentBuffer = 0;
  lastBuffer = 0;
  for (int i = 0; i < CAPTURE_PINS; i++) {
    memset(buffer[i].capture_buf, 0, CAPTURE_BUFFER_SIZE);
    buffer[i].sampleTime_us = SAMPLE_TIME_US * CAPTURE_PINS;
  }
  return true;
}

bool AnalogDmaRead::executeTask() {
  setupDMA();
  gatherSamples();
  return true;
}