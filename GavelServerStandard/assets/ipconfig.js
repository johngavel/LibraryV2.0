
// ipconfig.js
// UI logic for ipconfig.html: load current settings, validate, save, and reboot.

(function () {
  // ------- DOM helpers -------
  const $ = (id) => document.getElementById(id);

  // Inputs and UI elements
  const dhcp            = $('dhcp');
  const ipAddress       = $('ipAddress');
  const subnetMask      = $('subnetMask');
  const gatewayAddress  = $('gatewayAddress');
  const dnsAddress      = $('dnsAddress');
  const macAddress      = $('macAddress');
  const currentIp       = $('currentIp');
  const msg             = $('msg');
  const form            = $('ipForm');
  const btnReload       = $('btnReload');
  const btnSaveReboot   = $('btnSaveReboot');

  // ------- Validators -------
  const isIPv4 = (s) => {
    if (typeof s !== 'string') return false;
    if (!/^\d{1,3}(\.\d{1,3}){3}$/.test(s)) return false;
    return s.split('.').every(n => {
      const v = +n;
      return Number.isInteger(v) && v >= 0 && v <= 255;
    });
  };

  const VALID_MASKS = new Set([
    '255.0.0.0','255.128.0.0','255.192.0.0','255.224.0.0','255.240.0.0','255.248.0.0','255.252.0.0','255.254.0.0',
    '255.255.0.0','255.255.128.0','255.255.192.0','255.255.224.0','255.255.240.0','255.255.248.0','255.255.252.0','255.255.254.0',
    '255.255.255.0','255.255.255.128','255.255.255.192','255.255.255.224','255.255.255.240','255.255.255.248','255.255.255.252','255.255.255.254','255.255.255.255'
  ]);
  const isMask = (s) => isIPv4(s) && VALID_MASKS.has(s);

  // ------- UI helpers -------
  function setStaticInputsEnabled(enabled) {
    [ipAddress, subnetMask, gatewayAddress, dnsAddress].forEach(i => {
      i.disabled = !enabled;
      i.required = enabled;
    });
  }

  function showMsg(kind, text) {
    // kind: "", "ok", "err"
    msg.className = `msg ${kind}`.trim();
    msg.textContent = text || '';
    // Display if we have content or explicit kind
    msg.style.display = text || kind ? 'block' : 'none';
  }

  // ------- Load current info -------
  async function load() {
    try {
      showMsg('', '');
      // Uses existing helper from ipinfo.js (already included on the page)
      const info = await IpInfo.getIpInfo();

      // Device Info
      macAddress.value = info.macAddress || 'unknown';
      currentIp.value  = info.ipAddress || 'unknown';

      // Form fields
      const dhcpMode = !!info.isDHCP;
      dhcp.checked = dhcpMode;

      ipAddress.value      = (info.ipAddress      && info.ipAddress      !== 'unknown') ? info.ipAddress      : '';
      subnetMask.value     = (info.subnetMask     && info.subnetMask     !== 'unknown') ? info.subnetMask     : '';
      gatewayAddress.value = (info.gatewayAddress && info.gatewayAddress !== 'unknown') ? info.gatewayAddress : '';
      dnsAddress.value     = (info.dnsAddress     && info.dnsAddress     !== 'unknown') ? info.dnsAddress     : '';

      setStaticInputsEnabled(!dhcpMode);
    } catch (err) {
      showMsg('err', `Could not load current IP settings: ${err.message || err}`);
    }
  }

  // ------- Common payload builder + validation -------
  function validateAndBuildPayload() {
    const dhcpMode = dhcp.checked;

    if (!dhcpMode) {
      if (!isIPv4(ipAddress.value))       return { error: 'Please enter a valid IPv4 address.' };
      if (!isMask(subnetMask.value))      return { error: 'Please enter a valid subnet mask.' };
      if (gatewayAddress.value && !isIPv4(gatewayAddress.value))
                                          return { error: 'Gateway must be a valid IPv4 address.' };
      if (dnsAddress.value && !isIPv4(dnsAddress.value))
                                          return { error: 'DNS must be a valid IPv4 address.' };
    }

    return {
      payload: {
        isDHCP: dhcpMode,
        ipAddress:      dhcpMode ? null : ipAddress.value.trim(),
        subnetMask:     dhcpMode ? null : subnetMask.value.trim(),
        gatewayAddress: dhcpMode ? null : gatewayAddress.value.trim(),
        dnsAddress:     dhcpMode ? null : dnsAddress.value.trim(),
      }
    };
  }

  // ------- Save settings -------
  async function saveSettings(payload) {
    const resp = await fetch('/api/ip-config', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json', 'Accept': 'application/json' },
      body: JSON.stringify(payload),
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    return resp.json().catch(() => ({}));
  }

  // ------- Reboot device -------
  async function rebootDevice() {
    //  Send POST request to /reboot endpoint
    const resp = await fetch('/api/reboot.json', {
      method: 'POST',
      headers: { 'Accepted': 'application/json' }
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    return resp.text().catch(() => '');
  }

  // ------- Event: DHCP toggle -------
  dhcp.addEventListener('change', () => setStaticInputsEnabled(!dhcp.checked));

  // ------- Event: Reload -------
  btnReload?.addEventListener('click', load);

  // ------- Event: Save Settings (form submit) -------
  form?.addEventListener('submit', async (ev) => {
    ev.preventDefault();

    showMsg('', '');
    const check = validateAndBuildPayload();
    if (check.error) return showMsg('err', check.error);

    try {
      const result = await saveSettings(check.payload);
      showMsg('ok', result.message || 'Network settings updated successfully.');
    } catch (err) {
      showMsg('err', `Failed to save settings: ${err.message || err}`);
    }
  });

  // ------- Event: Save & Reboot -------
  btnSaveReboot?.addEventListener('click', async () => {
    showMsg('', '');
    const check = validateAndBuildPayload();
    if (check.error) return showMsg('err', check.error);

    // Show confirmation dialog before rebooting
    const confirmed = confirm(
      'Are you sure you want to reboot the device?\n\nActive sessions and operations may be interrupted.'
    );
    if (!confirmed) return;

    try {
      // Save first
      await saveSettings(check.payload);
      showMsg('ok', 'Settings saved. Rebooting device…');

      // Reboot
      await rebootDevice();
      showMsg('ok', 'Save & Reboot initiated. The device may be temporarily unavailable.');
      let remaining = 10;
      const ticker = setInterval(() => { remaining--; if (remaining <= 0) clearInterval(ticker); }, 1000);
      setTimeout(() => { window.location.replace('/index.html'); }, 10000); // redirect after 10s
    } catch (err) {
      showMsg('err', err.message || String(err));
    }
  });

  // ------- Initial load -------
  load();
})();
