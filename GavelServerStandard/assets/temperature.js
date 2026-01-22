
// temperature-widget.js
/**
 * <temperature-widget>
 * External CSS controls all styling.
 * Fixed logic:
 *   - Endpoint: /api/temperature.json
 *   - Polling: 60 seconds
 *   - Shows: "Temperature: <rounded> °F"
 *   - Hides when tempvalid is false or on errors
 */

export class TemperatureWidget extends HTMLElement {
  constructor() {
    super();

    // No shadow DOM – allow external CSS to style freely
    this._container = document.createElement('span');
    this.appendChild(this._container);

    this._intervalId = null;
    this._abortController = null;

    // Fixed settings
    this._ENDPOINT = '/api/temperature.json';
    this._POLL_MS = 60_000;
  }

  connectedCallback() {
    this.start();
  }

  disconnectedCallback() {
    this.stop();
  }

  start() {
    if (this._intervalId != null) return;
    this._fetchAndRender();
    this._intervalId = window.setInterval(() => this._fetchAndRender(), this._POLL_MS);
  }

  stop() {
    if (this._intervalId != null) {
      clearInterval(this._intervalId);
      this._intervalId = null;
    }
    if (this._abortController) {
      this._abortController.abort();
      this._abortController = null;
    }
  }

  refresh() {
    this._fetchAndRender();
  }

  async _fetchAndRender() {
    if (this._abortController) {
      this._abortController.abort();
    }
    this._abortController = new AbortController();

    try {
      const url = new URL(this._ENDPOINT, window.location.origin);
      url.searchParams.set('_', Date.now());

      const res = await fetch(url.toString(), {
        cache: 'no-store',
        headers: { 'Accept': 'application/json' },
        signal: this._abortController.signal
      });

      if (!res.ok) throw new Error(`HTTP ${res.status}`);

      const data = await res.json();
      const { tempdrift, tempvalid, temperature } = data ?? {};

      if (tempvalid === true && isFinite(temperature) && isFinite(tempdrift)) {
        const adjusted = Math.round(Number(temperature) - Number(tempdrift));
        this._container.textContent = `Temperature: ${adjusted} °F`;
        this.style.display = '';
      } else {
        this._hide();
      }
    } catch {
      this._hide();
    }
  }

  _hide() {
    this._container.textContent = '';
    this.style.display = 'none';
  }
}

if (!customElements.get('my-temperature')) {
  customElements.define('my-temperature', TemperatureWidget);
}
