// ip-redirect.js
// Minimal web component: fetch IPv4 from JSON and set HTTP meta refresh.
// - IPv4-only validation
// - Requires a <meta> tag; no JS redirect fallback
// - No custom events

export class IpRedirect extends HTMLElement {
  static get observedAttributes() {
    return ['src', 'meta-id', 'delay', 'field'];
  }

  connectedCallback() {
    this.#run();
  }
  attributeChangedCallback() {
    this.#run();
  }

  async #run() {
    const src = this.getAttribute('src') || '/api/ip-info.json';
    const metaId = this.getAttribute('meta-id') || 'refresh-meta';
    const delay = this.#int(this.getAttribute('delay'), 3);
    const field = this.getAttribute('field') || 'ipAddress';

    if (!src) return;

    const meta = document.getElementById(metaId);
    if (!meta) return; // zero-fallback: do nothing if meta not present

    try {
      const res = await fetch(src, {
        cache: 'no-store'
      });
      if (!res.ok) return;

      const data = await res.json();
      const ip = (data?.[field] ?? '').toString().trim();

      const ipv4Pattern =
        /^(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}$/;

      if (!ipv4Pattern.test(ip)) return;

      meta.setAttribute('http-equiv', 'refresh');
      meta.setAttribute('content', `${delay}; url=http://${ip}`);
    } catch (_) {
      // minimal: silently ignore
    }
  }

  #int(v, d) {
    const n = parseInt(v ?? '', 10);
    return Number.isFinite(n) && n > 0 ? n : d;
  }
}

if (!customElements.get('ip-redirect')) {
  customElements.define('ip-redirect', IpRedirect);
}