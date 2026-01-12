const DEFAULT_LINK = '#';

export class NavButton extends HTMLElement {
  static get observedAttributes() {
    return ['link', 'color', 'disabled'];
  }

  constructor() {
    super();

    // Create the internal structure in light DOM (no shadow)
    this._button = document.createElement('button');
    this._button.type = 'button';
    this._button.classList.add('nav-button'); // style via style.css
    this._button.part = 'button'; // optional external styling: ::part(button)

    // Move any existing light‑DOM text or children inside the button
    // so <nav-button>Label</nav-button> shows as button text.
    if (this.childNodes.length > 0) {
      // Wrap current children inside the button
      const frag = document.createDocumentFragment();
      while (this.firstChild) frag.appendChild(this.firstChild);
      this._button.appendChild(frag);
    } else {
      // Default label if none provided
      this._button.textContent = 'Navigate';
    }

    this.appendChild(this._button);

    // Click navigation behavior
    this._button.addEventListener('click', (e) => {
      if (this.disabled) return;
      const url = this.link || DEFAULT_LINK;

      // Allow ctrl/meta/middle-click to open in a new tab
      const openNewTab = e.ctrlKey || e.metaKey || e.button === 1;
      try {
        if (openNewTab) {
          window.open(url, '_blank', 'noopener');
        } else {
          window.location.href = url;
        }
      } catch (err) {
        console.error('nav-button navigation failed:', err);
      }
    });
  }

  // --- Properties ---
  get link() {
    return this.getAttribute('link') || DEFAULT_LINK;
  }
  set link(v) {
    v == null ? this.removeAttribute('link') : this.setAttribute('link', String(v));
  }

  // Expose color to CSS via data attribute for authors to target in style.css
  get color() {
    return this.getAttribute('color') || '';
  }
  set color(v) {
    v == null ? this.removeAttribute('color') : this.setAttribute('color', String(v));
  }

  get disabled() {
    return this.hasAttribute('disabled');
  }
  set disabled(v) {
    v ? this.setAttribute('disabled', '') : this.removeAttribute('disabled');
  }

  attributeChangedCallback(name, _old, val) {
    switch (name) {
      case 'color':
        if (val == null) this._button.removeAttribute('data-color');
        else this._button.setAttribute('data-color', val);
        break;
      case 'disabled':
        this._button.toggleAttribute('disabled', this.disabled);
        break;
      case 'link':
        // Nothing to reflect visually; navigation uses the current value
        break;
    }
  }

  connectedCallback() {
    // Initialize disabled + color state
    this._button.toggleAttribute('disabled', this.disabled);
    if (this.color) this._button.setAttribute('data-color', this.color);
  }
}

export class RebootButton extends NavButton {
  static get observedAttributes() {
    // Inherit the base attributes and add reboot-specific ones
    return [...super.observedAttributes, 'endpoint', 'redirect', 'delay'];
  }

  constructor() {
    super();

    // Default label if none was provided via light DOM
    if (!this._button || !this._button.textContent || this._button.textContent.trim() === '') {
      this._button.textContent = 'Reboot';
    }

    // Bind the handler so we can add/remove it
    this._onRebootClick = this._onRebootClick.bind(this);

    /**
     * Important: Add our listener in the **capture** phase.
     * That allows us to intercept and cancel the base class navigation handler
     * that was attached in NavButton’s constructor (bubble phase).
     */
    this._button.addEventListener('click', this._onRebootClick, {
      capture: true
    });
  }

  // --- Reboot-specific attributes/properties ---

  get endpoint() {
    return this.getAttribute('endpoint') || '/api/reboot.json';
  }
  set endpoint(v) {
    v == null ? this.removeAttribute('endpoint') : this.setAttribute('endpoint', String(v));
  }

  get redirect() {
    return this.getAttribute('redirect') || '/index.html';
  }
  set redirect(v) {
    v == null ? this.removeAttribute('redirect') : this.setAttribute('redirect', String(v));
  }

  get delay() {
    const d = this.getAttribute('delay');
    return d == null ? 5000 : Number(d);
  }
  set delay(v) {
    v == null ? this.removeAttribute('delay') : this.setAttribute('delay', String(v));
  }

  // --- Click handler that overrides base navigation ---
  async _onRebootClick(e) {
    // Stop NavButton’s navigation behavior
    e.preventDefault();
    e.stopImmediatePropagation();

    if (this.disabled) return;

    const confirmed = window.confirm(
      'Are you sure you want to reboot the device?\n\nActive sessions and operations may be interrupted.'
    );
    if (!confirmed) return;

    const originalText = this._button.textContent;
    this._button.disabled = true;
    this._button.textContent = 'Rebooting…';

    try {
      const resp = await fetch(this.endpoint, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
      });

      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);

      // Redirect after delay
      setTimeout(() => {
        window.location.replace(this.redirect);
      }, this.delay);
    } catch (err) {
      console.error('reboot-button failed:', err);
      alert('Failed to send reboot command. Please try again or check server logs.');
      this._button.disabled = false;
      this._button.textContent = originalText || 'Reboot';
      this._button.focus();
    }
  }

  // Clean up if detached
  disconnectedCallback() {
    if (this._button && this._onRebootClick) {
      this._button.removeEventListener('click', this._onRebootClick, {
        capture: true
      });
    }
    super.disconnectedCallback?.();
  }
}

export class BackButton extends NavButton {
  constructor() {
    super();

    // Default label if none provided
    if (!this._button || !this._button.textContent.trim()) {
      this._button.textContent = 'Back';
    }

    // Override click behavior
    this._onBackClick = this._onBackClick.bind(this);
    this._button.addEventListener('click', this._onBackClick, {
      capture: true
    });
  }

  _onBackClick(e) {
    // Stop NavButton’s default navigation
    e.preventDefault();
    e.stopImmediatePropagation();

    if (this.disabled) return;

    try {
      // Go back in history; fallback to link if no history
      if (window.history.length > 1) {
        window.history.back();
      } else {
        const fallback = this.link || '#';
        window.location.href = fallback;
      }
    } catch (err) {
      console.error('back-button navigation failed:', err);
    }
  }

  disconnectedCallback() {
    if (this._button && this._onBackClick) {
      this._button.removeEventListener('click', this._onBackClick, {
        capture: true
      });
    }
    super.disconnectedCallback?.();
  }
}

export class DownloadButton extends NavButton {
  static get observedAttributes() {
    return [...super.observedAttributes, 'url', 'filename'];
  }

  constructor() {
    super();

    if (!this._button || !this._button.textContent.trim()) {
      this._button.textContent = 'Download';
    }

    this._onClick = this._onClick.bind(this);
    this._button.addEventListener('click', this._onClick, {
      capture: true
    });
  }

  // --- Attributes / properties ---
  get url() {
    return this.getAttribute('url') || this.link || '/api/download.json';
  }
  set url(v) {
    v == null ? this.removeAttribute('url') : this.setAttribute('url', String(v));
  }

  get filename() {
    return this.getAttribute('filename') || 'download';
  }
  set filename(v) {
    v == null ? this.removeAttribute('filename') : this.setAttribute('filename', String(v));
  }

  // --- Behavior ---
  async _onClick(e) {
    e.preventDefault();
    e.stopImmediatePropagation();
    if (this.disabled) return;

    const originalText = this._button.textContent;
    this._button.disabled = true;
    this._button.textContent = 'Downloading…';
    this._button.setAttribute('aria-busy', 'true');

    try {
      // JSON-only, POST-only trigger
      const resp = await fetch(this.url, {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json'
        },
      });

      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);

      const blob = await resp.blob();
      const objectUrl = URL.createObjectURL(blob);

      const a = document.createElement('a');
      a.href = objectUrl;
      a.download = this.filename;
      document.body.appendChild(a);
      a.click();
      a.remove();
      URL.revokeObjectURL(objectUrl);

      this.dispatchEvent(new CustomEvent('download-success', {
        detail: {
          response: resp,
          filename: this.filename
        },
        bubbles: true
      }));
    } catch (err) {
      console.error('download-button error:', err);
      this.dispatchEvent(new CustomEvent('download-error', {
        detail: {
          error: err
        },
        bubbles: true
      }));
      alert('Download failed.');
    } finally {
      this._button.disabled = false;
      this._button.textContent = originalText || 'Download';
      this._button.removeAttribute('aria-busy');
    }
  }

  disconnectedCallback() {
    if (this._button && this._onClick) {
      this._button.removeEventListener('click', this._onClick, {
        capture: true
      });
    }
    super.disconnectedCallback?.();
  }
}

export class UploadButton extends NavButton {
  static get observedAttributes() {
    return [...super.observedAttributes, 'endpoint', 'accept'];
  }

  constructor() {
    super();

    if (!this._button || !this._button.textContent.trim()) {
      this._button.textContent = 'Upload';
    }

    this._onClick = this._onClick.bind(this);
    this._button.addEventListener('click', this._onClick, {
      capture: true
    });

    // Hidden single-file input
    this._input = document.createElement('input');
    this._input.type = 'file';
    this._input.style.display = 'none';
    this._input.multiple = false; // enforce single file

    // Default to JSON data only
    this._defaultAccept =
      'application/json,application/ld+json,.json';

    this._input.addEventListener('change', () =>
      this._handleFile(this._input.files?.[0] || null)
    );
    this.appendChild(this._input);
  }

  // --- Attributes / properties ---
  get endpoint() {
    return this.getAttribute('endpoint') || '/api/upload.json';
  }
  set endpoint(v) {
    v == null ? this.removeAttribute('endpoint') : this.setAttribute('endpoint', String(v));
  }

  get accept() {
    // If author sets 'accept', use it; otherwise JSON-only defaults
    return this.getAttribute('accept') || this._defaultAccept;
  }
  set accept(v) {
    v == null ? this.removeAttribute('accept') : this.setAttribute('accept', String(v));
  }

  attributeChangedCallback(name, oldV, newV) {
    super.attributeChangedCallback?.(name, oldV, newV);
    if (!this._input) return;
    if (name === 'accept') this._input.accept = this.accept;
  }

  connectedCallback() {
    super.connectedCallback?.();
    if (this._input) {
      this._input.accept = this.accept; // default or author override
      this._input.multiple = false; // ensure single file
    }
  }

  // --- Behavior ---
  _onClick(e) {
    e.preventDefault();
    e.stopImmediatePropagation();
    if (this.disabled) return;
    this._input.click();
  }

  async _handleFile(file) {
    if (!file) return;

    const originalText = this._button.textContent;
    this._button.disabled = true;
    this._button.textContent = 'Uploading…';
    this._button.setAttribute('aria-busy', 'true');

    try {
      // Read JSON as text (no ArrayBuffer, no DataURL)
      const text = await this._readAsText(file);

      // Optionally verify it looks like JSON (non-fatal if it doesn’t)
      let parsed = null;
      try {
        parsed = JSON.parse(text);
      } catch {
        /* ignore parse errors */
      }

      // JSON-only, POST-only payload
      const payload = {
        file: {
          name: file.name,
          type: file.type || 'application/json',
          size: file.size,
          text, // raw JSON text as a string
          // parsed, // uncomment if your server wants a parsed object (beware large payloads)
        },
      };

      const resp = await fetch(this.endpoint, {
        method: 'POST', // always POST
        headers: {
          'Content-Type': 'application/json'
        }, // JSON-only
        body: JSON.stringify(payload),
      });

      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);

      let data = null;
      try {
        data = await resp.json();
      } catch {
        /* server may return empty body */
      }

      this.dispatchEvent(new CustomEvent('upload-success', {
        detail: {
          response: resp,
          data
        },
        bubbles: true
      }));
    } catch (err) {
      console.error('upload-button error:', err);
      this.dispatchEvent(new CustomEvent('upload-error', {
        detail: {
          error: err
        },
        bubbles: true
      }));
      alert('Upload failed.');
    } finally {
      this._button.disabled = false;
      this._button.textContent = originalText || 'Upload';
      this._button.removeAttribute('aria-busy');
      this._input.value = ''; // reset picker
      window.location.reload(true);
    }
  }

  _readAsText(file) {
    return new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onerror = () => reject(reader.error);
      reader.onload = () => resolve(String(reader.result)); // plain text string
      reader.readAsText(file);
    });
  }

  disconnectedCallback() {
    if (this._button && this._onClick) {
      this._button.removeEventListener('click', this._onClick, {
        capture: true
      });
    }
    super.disconnectedCallback?.();
  }
}

export class UpgradeButton extends NavButton {
  static get observedAttributes() {
    return [
      ...super.observedAttributes,
      'upload-url', // default: /var/pico.bin
      'upgrade-url', // default: /api/upgrade.json
      'accept', // default: application/octet-stream,.bin
      'max-size', // default: 32 MiB
      'delay', // default: 5000 ms
      'redirect' // default: /index.html
    ];
  }

  constructor() {
    super();

    // Default label
    if (!this._button || !this._button.textContent?.trim()) {
      this._button.textContent = 'Upgrade';
    }

    // Capture‑phase listener to override NavButton navigation
    this._onClick = this._onClick.bind(this);
    this._button.addEventListener('click', this._onClick, {
      capture: true
    });
  }

  // --- Attributes / Properties ---
  get uploadUrl() {
    return this.getAttribute('upload-url') || '/var/pico.bin';
  }
  set uploadUrl(v) {
    v == null ? this.removeAttribute('upload-url') : this.setAttribute('upload-url', String(v));
  }

  get upgradeUrl() {
    return this.getAttribute('upgrade-url') || '/api/upgrade.json';
  }
  set upgradeUrl(v) {
    v == null ? this.removeAttribute('upgrade-url') : this.setAttribute('upgrade-url', String(v));
  }

  get accept() {
    return this.getAttribute('accept') || 'application/octet-stream,.bin';
  }
  set accept(v) {
    v == null ? this.removeAttribute('accept') : this.setAttribute('accept', String(v));
  }

  get maxSize() {
    const v = this.getAttribute('max-size');
    return v == null ? (1 * 1024 * 1024) : Number(v);
  }
  set maxSize(v) {
    v == null ? this.removeAttribute('max-size') : this.setAttribute('max-size', String(v));
  }

  get delay() {
    const v = this.getAttribute('delay');
    return v == null ? 5000 : Number(v);
  }
  set delay(v) {
    v == null ? this.removeAttribute('delay') : this.setAttribute('delay', String(v));
  }

  get redirect() {
    return this.getAttribute('redirect') || '/index.html';
  }
  set redirect(v) {
    v == null ? this.removeAttribute('redirect') : this.setAttribute('redirect', String(v));
  }

  // --- Click → pick file → upload → upgrade ---
  _onClick(e) {
    e.preventDefault();
    e.stopImmediatePropagation();
    if (this.disabled) return;

    // One‑off hidden file input (single file)
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = this.accept;
    input.multiple = false;
    input.style.display = 'none';
    this.appendChild(input);

    const cleanup = () => input.remove();

    input.addEventListener('change', async () => {
      try {
        const file = input.files?.[0] || null;
        cleanup();
        if (!file) return; // user canceled

        // Basic validations
        if (!/\.bin$/i.test(file.name)) {
          alert('Please choose a .bin firmware file.');
          return;
        }
        if (file.size === 0) {
          alert('Selected file is empty.');
          return;
        }
        if (file.size > this.maxSize) {
          alert(`File too large (${file.size} bytes). Max allowed is ${this.maxSize} bytes.`);
          return;
        }

        const originalText = this._button.textContent;
        this._setBusy(true, 'Uploading…');

        // Upload firmware (fetch-only, no metadata headers)
        const respUpload = await fetch(this.uploadUrl, {
          method: 'POST',
          body: file
        });

        if (!respUpload.ok) {
          const text = await this._safeText(respUpload);
          throw new Error(`Upload failed: HTTP ${respUpload.status} ${respUpload.statusText}${text ? ` — ${text}` : ''}`);
        }

        // Apply/upgrade command (POST, no headers)
        this._setBusy(true, 'Applying…');
        const respUpgrade = await fetch(this.upgradeUrl, {
          method: 'POST'
        });

        if (!respUpgrade.ok) {
          const text = await this._safeText(respUpgrade);
          throw new Error(`Upgrade failed: HTTP ${respUpgrade.status} ${respUpgrade.statusText}${text ? ` — ${text}` : ''}`);
        }

        // Optional redirect after short delay
        setTimeout(() => window.location.replace(this.redirect), this.delay);

      } catch (err) {
        console.error('upgrade-button error:', err);
        alert(err?.message || 'Upgrade failed. Please check server logs and try again.');
      } finally {
        this._setBusy(false, 'Upgrade');
      }
    }, {
      once: true
    });

    input.click();
  }

  _setBusy(busy, label) {
    this._button.disabled = busy;
    this._button.textContent = label ?? (busy ? 'Working…' : 'Upgrade');
    this._button.toggleAttribute('aria-busy', busy);
  }

  async _safeText(resp) {
    try {
      return await resp.text();
    } catch {
      return '';
    }
  }

  disconnectedCallback() {
    if (this._button && this._onClick) {
      this._button.removeEventListener('click', this._onClick, {
        capture: true
      });
    }
    super.disconnectedCallback?.();
  }
}

// Define the element tag
customElements.define('upgrade-button', UpgradeButton);
customElements.define('upload-button', UploadButton);
customElements.define('download-button', DownloadButton);
customElements.define('back-button', BackButton);
customElements.define('reboot-button', RebootButton);
customElements.define('nav-button', NavButton);