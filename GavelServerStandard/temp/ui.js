export class ConfigCard extends HTMLElement {
  static get observedAttributes() {
    return ['title']; // we’ll map title -> legend text
  }

  constructor() {
    super();

    // Use fieldset for grouped controls
    this._fieldset = document.createElement('fieldset');
    this._fieldset.classList.add('card'); // style in style.css

    // Native legend
    this._legendEl = document.createElement('legend');
    this._legendEl.classList.add('card-legend'); // style in style.css
    this._fieldset.appendChild(this._legendEl);

    // Title element (optional separate heading if you still want a header)
    this._titleEl = document.createElement('h2');
    this._titleEl.classList.add('card-title');
    this._fieldset.appendChild(this._titleEl);

    // Move any existing children inside the fieldset
    const frag = document.createDocumentFragment();
    while (this.firstChild) frag.appendChild(this.firstChild);
    this._fieldset.appendChild(frag);

    this.appendChild(this._fieldset);
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
      const t = val ?? '';
      // Map title to both legend and heading for clarity
      this._legendEl.textContent = t;
      this._titleEl.textContent = t;
      // If you prefer only the legend, you can hide or remove _titleEl
      this._titleEl.hidden = true; // keep legend as the visible “title”
    }
  }

  connectedCallback() {
    const t = this.title;
    this._legendEl.textContent = t;
    this._titleEl.textContent = t;
    this._titleEl.hidden = true;
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
customElements.define('element-group', ElementGroup);