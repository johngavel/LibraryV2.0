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

export class ConfigCard extends HTMLElement {
  static get observedAttributes() {
    return ['title'];
  }

  constructor() {
    super();

    // Build light-DOM structure (no shadow, no inline styles)
    this._section = document.createElement('section');
    this._section.classList.add('card'); // style in style.css

    // Title element
    this._titleEl = document.createElement('h2');
    this._titleEl.classList.add('card-title'); // style in style.css
    this._section.appendChild(this._titleEl);

    // Move any existing children after the title inside the section.
    // (Children could include your buttons-group or any other content.)
    const frag = document.createDocumentFragment();
    while (this.firstChild) frag.appendChild(this.firstChild);
    this._section.appendChild(frag);

    // Attach to this element
    this.appendChild(this._section);
  }

  get title() {
    return this.getAttribute('title') || '';
  }
  set title(val) {
    if (val == null) this.removeAttribute('title');
    else this.setAttribute('title', String(val));
  }

  attributeChangedCallback(name, _old, val) {
    if (name === 'title') {
      this._titleEl.textContent = val ?? '';
    }
  }

  connectedCallback() {
    // Initialize title on first connect
    if (this.title) {
      this._titleEl.textContent = this.title;
    }
  }
}

export class ElementGroup extends HTMLElement {
  static get observedAttributes() {
    return ['legend']; // <buttons-group legend="Actions">…</buttons-group>
  }

  constructor() {
    super();

    // Light-DOM only: fieldset + legend (no shadow, no inline styles, no aria)
    this._fieldset = document.createElement('fieldset');
    this._fieldset.classList.add('element-group'); // style in style.css

    this._legend = document.createElement('legend');
    this._legend.classList.add('element-legend'); // style in style.css
    this._fieldset.appendChild(this._legend);

    // Move current children (e.g., <nav-button> items) into the fieldset
    const frag = document.createDocumentFragment();
    while (this.firstChild) frag.appendChild(this.firstChild);
    this._fieldset.appendChild(frag);

    // Attach
    this.appendChild(this._fieldset);
  }

  // Attribute/property API
  get legend() {
    return this.getAttribute('legend') || '';
  }
  set legend(val) {
    if (val == null) this.removeAttribute('legend');
    else this.setAttribute('legend', String(val));
  }

  attributeChangedCallback(name, _old, val) {
    if (name === 'legend') {
      this._legend.textContent = val ?? '';
      // If legend is empty, you can optionally hide the element for cleaner markup:
      this._legend.style.display = val ? '' : 'none';
    }
  }

  connectedCallback() {
    // Initialize legend text
    this._legend.textContent = this.legend;
    this._legend.style.display = this.legend ? '' : 'none';
  }
}


customElements.define('config-card', ConfigCard);
customElements.define('nav-button', NavButton);
customElements.define('element-group', ElementGroup);