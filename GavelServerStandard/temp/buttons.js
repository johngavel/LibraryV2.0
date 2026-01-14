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

// Define the element tag
customElements.define('back-button', BackButton);
customElements.define('reboot-button', RebootButton);
customElements.define('nav-button', NavButton);