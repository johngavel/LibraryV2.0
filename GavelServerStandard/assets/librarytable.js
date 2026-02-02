// /js/library-table.element.js
// <library-table> rendered in LIGHT DOM (no Shadow DOM)

import {
  ServerInfo
} from '/js/serverinfo.js';

class LibraryTableElement extends HTMLElement {
  static get observedAttributes() {
    return ['src', 'count-target'];
  }

  constructor() {
    super();
    // Render directly to the element (light DOM)
    this._root = this; // no shadow
    this._countTargetSelector = null;
    this._src = '/api/license-info.json'; // default preserves original path
  }

  async connectedCallback() {
    try {
      const server = await ServerInfo.getServerInfo(); // <-- await the Promise
      if (server.licenseInfo) {
        // Read attributes (if present)
        if (this.hasAttribute('src')) {
          const s = this.getAttribute('src');
          if (s && s.trim()) this._src = s.trim();
        }
        if (this.hasAttribute('count-target')) {
          const sel = this.getAttribute('count-target');
          if (sel && sel.trim()) this._countTargetSelector = sel.trim();
        }

        // Initial render
        this.render();
      }
    } catch (err) {
      // Avoid throwing out of lifecycle; fail quietly and optionally log.
      console.error('[library-table] failed to initialize:', err);
    }
  }

  attributeChangedCallback(name, _oldVal, newVal) {
    if (name === 'src') {
      this._src = (newVal && newVal.trim()) ? newVal.trim() : this._src;
      this.render();
    } else if (name === 'count-target') {
      this._countTargetSelector = (newVal && newVal.trim()) ? newVal.trim() : null;
      this.render();
    }
  }

  // Status messages (same classes as your page CSS)
  insertStatus(message, type = 'warn') {
    const p = document.createElement('p');
    p.className = type; // 'warn' or 'error' (matches style.css)
    p.textContent = message;
    this._root.appendChild(p);
  }

  async render() {
    // Clear previous content
    this._root.innerHTML = '';

    // Fetch data (unchanged)
    let data;
    try {
      const res = await fetch(this._src, {});
      if (!res.ok) {
        this.insertStatus(`Failed to fetch license info (HTTP ${res.status}).`, 'error');
        return;
      }
      data = await res.json();
    } catch (err) {
      console.error(err);
      this.insertStatus('Unable to load license info (network or JSON parse error).', 'error');
      return;
    }

    // Validate input (unchanged)
    const libs = Array.isArray(data?.library) ? data.library : [];
    const count = Number.isFinite(data?.countLibrary) ? data.countLibrary : libs.length;

    // External header update if requested
    if (this._countTargetSelector) {
      const mainTitle = document.querySelector(this._countTargetSelector);
      if (mainTitle) mainTitle.textContent = `Libraries (${count})`;
    }

    if (libs.length === 0) {
      this.insertStatus('No libraries to display.');
      return;
    }

    // Build table (unchanged structure)
    const table = document.createElement('table');

    // THEAD
    const thead = document.createElement('thead');
    const headerRow = document.createElement('tr');
    ['Name', 'Version', 'License', 'Link'].forEach(text => {
      const th = document.createElement('th');
      th.scope = 'col';
      th.textContent = text;
      headerRow.appendChild(th);
    });
    thead.appendChild(headerRow);
    table.appendChild(thead);

    // TBODY
    const tbody = document.createElement('tbody');
    libs.forEach(item => {
      const tr = document.createElement('tr');

      // Name
      const nameTd = document.createElement('td');
      nameTd.textContent = item?.name ?? '';
      tr.appendChild(nameTd);

      // Version
      const versionTd = document.createElement('td');
      versionTd.textContent = item?.version ?? '';
      tr.appendChild(versionTd);

      // License (fallback to em dash)
      const licenseTd = document.createElement('td');
      const license = (item?.license_name && String(item.license_name).trim()) ? item.license_name : '—';
      licenseTd.textContent = license;
      tr.appendChild(licenseTd);

      // Link
      const linkTd = document.createElement('td');
      const href = item?.link;
      if (href && String(href).trim()) {
        const a = document.createElement('a');
        a.href = href;
        a.textContent = href;
        a.target = '_blank';
        a.rel = 'noopener noreferrer';
        linkTd.appendChild(a);
      } else {
        linkTd.textContent = '—';
      }
      tr.appendChild(linkTd);

      tbody.appendChild(tr);
    });

    table.appendChild(tbody);

    // Insert into element (light DOM)
    this._root.appendChild(table);
    // No shadow styles—use your global CSS
  }
}

// Register the element
customElements.define('library-table', LibraryTableElement);