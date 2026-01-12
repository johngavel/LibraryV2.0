// Import modules (adjust paths to match your project tree)
import {
  BuildInfo
} from '/js/buildinfo.js';
import {
  IpInfo
} from '/js/ipinfo.js';
import {
  escapeHtml
} from '/js/util.js';
import '/js/ui.js';
import '/js/buttons.js'

document.addEventListener('DOMContentLoaded', () => {
  console.log('Components loaded');
});

export class MyHeader extends HTMLElement {
  constructor() {
    super();
    // Optional: set up Shadow DOM if you want style encapsulation
    // this.attachShadow({ mode: 'open' });
  }

  async connectedCallback() {
    // Render the shell first so the element appears immediately
    this.innerHTML = `
      <header id="site-header"></header>
    `;

    // Now perform async work
    try {
      const info = await BuildInfo.getBuildInfo();
      document.title = `${info.product}`;

      // Target the header inside THIS component (safer than querying document)
      const hostEl = this.querySelector('#site-header');
      if (!hostEl) return;

      hostEl.innerHTML = `
        <h1>${escapeHtml(info.product)}</h1>
        <hr style="width:50%">
      `;
    } catch (err) {
      console.error('Failed to load build info:', err);
    }
  }
}

export class MyFooter extends HTMLElement {
  constructor() {
    super();
    // Optional: set up Shadow DOM if you want style encapsulation
    // this.attachShadow({ mode: 'open' });
  }

  async connectedCallback() {
    // Render the shell first so the element appears immediately
    this.innerHTML = `
      <footer id="site-footer"></footer>
    `;

    // Now perform async work
    try {
      const info = await BuildInfo.getBuildInfo();

      // Target the header inside THIS component (safer than querying document)
      const hostEl = this.querySelector('#site-footer');
      if (!hostEl) return;

      hostEl.innerHTML = `
    <div class="footer-content">
      <div>${escapeHtml(info.product)}</div>
      <div>Ver. ${escapeHtml(info.version)}</div>
      <div>Build Date: ${escapeHtml(info.build_date)} Time: ${escapeHtml(info.build_time)}</div>
      <div>Author: ${escapeHtml(info.author)}</div>
    </div>
      `;
    } catch (err) {
      console.error('Failed to load build info:', err);
    }
  }
}

customElements.define('my-header', MyHeader);
customElements.define('my-footer', MyFooter);