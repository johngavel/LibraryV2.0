// data-table.element.js (clean: no domain-specific registries)
class DataTableElement extends HTMLElement {
  static get observedAttributes() {
    return ['src', 'row-key', 'interval-ms', 'columns', 'summary'];
  }
  constructor() {
    super();
    this._root = this;
    this._src = '';
    this._rowKey = 'auto';
    this._interval = 2000;
    this._columns = [];
    this._summarySpec = '';
    this._refreshHandle = null;
    this._busy = false;
    this._summarySpan = null;
    this._table = null;
    this._tbody = null;
    this._rowIndex = new Map();
  }
  connectedCallback() {
    this._readAttributes();
    if (!this._summarySpan) this._buildShell();
    this.render();
    if (!this._refreshHandle && this._src) {
      this._refreshHandle = setInterval(() => this.render(), this._interval);
    }
  }
  disconnectedCallback() {
    if (this._refreshHandle) {
      clearInterval(this._refreshHandle);
      this._refreshHandle = null;
    }
  }
  attributeChangedCallback() {
    this._readAttributes();
    this.render();
  }
  _readAttributes() {
    const get = (n, d = '') => (this.hasAttribute(n) ? this.getAttribute(n) : d);
    const num = (v, d) => Number.isFinite(Number(v)) ? Number(v) : d;
    const src = get('src').trim();
    if (src) this._src = src;
    const rk = get('row-key').trim();
    this._rowKey = rk || 'auto';
    this._interval = num(get('interval-ms'), this._interval || 2000);
    const cols = get('columns');
    if (cols) {
      try {
        this._columns = JSON.parse(cols);
      } catch {}
    }
    const summary = get('summary');
    if (summary) {
      try {
        this._summarySpec = JSON.parse(summary);
      } catch {
        this._summarySpec = summary;
      }
    }
  }
  _buildShell() {
    const container = document.createElement('div');
    this._summarySpan = document.createElement('span');
    container.appendChild(this._summarySpan);
    this._table = document.createElement('table');
    const thead = document.createElement('thead');
    const headerRow = document.createElement('tr');
    (this._columns || []).forEach(col => {
      const th = document.createElement('th');
      th.scope = 'col';
      th.textContent = col.header || '';
      headerRow.appendChild(th);
    });
    thead.appendChild(headerRow);
    this._table.appendChild(thead);
    this._tbody = document.createElement('tbody');
    this._table.appendChild(this._tbody);
    container.appendChild(this._table);
    this._root.replaceChildren(container);
  }
  insertStatus(message, type = 'warn') {
    const prior = this._root.querySelectorAll(':scope p.warn, :scope p.error');
    prior.forEach(p => p.remove());
    const p = document.createElement('p');
    p.className = type;
    p.textContent = message;
    if (this._summarySpan?.parentNode) this._summarySpan.after(p);
    else this._root.appendChild(p);
  }
  _get(obj, path) {
    if (!path) return obj;
    return path.split('.').reduce((acc, k) => acc?.[k], obj);
  }
  _getKey(item) {
    if (this._rowKey && this._rowKey !== 'auto') {
      const val = this._get(item, this._rowKey);
      return String(val ?? '');
    }
    for (const k of ['id', 'macAddress', 'ipAddress', 'hwid', 'hwname']) {
      const v = item?.[k];
      if (v !== undefined && v !== null && v !== '') return String(v);
    }
    return JSON.stringify(item);
  }
  _createRow(item, key) {
    const tr = document.createElement('tr');
    tr.dataset.key = key;
    const cells = [];
    (this._columns || []).forEach(col => {
      const td = document.createElement('td');
      const val = this._get(item, col.field);
      const textFn = window._datatableFns?.[col.text];
      td.textContent = textFn ? String(textFn(val, item, col, this) ?? '') : String(val ?? '');
      const classFn = window._datatableFns?.[col.classFor];
      if (classFn) {
        const cls = classFn(val, item, col, this);
        td.className = cls || '';
      }
      cells.push(td);
      tr.appendChild(td);
    });
    tr._cells = {
      cells
    };
    return tr;
  }
  _updateRow(tr, item) {
    const {
      cells
    } = tr._cells;
    (this._columns || []).forEach((col, i) => {
      const td = cells[i];
      const val = this._get(item, col.field);
      const textFn = window._datatableFns?.[col.text];
      const nextText = textFn ? String(textFn(val, item, col, this) ?? '') : String(val ?? '');
      if (td.textContent !== nextText) td.textContent = nextText;
      const classFn = window._datatableFns?.[col.classFor];
      const nextClass = classFn ? (classFn(val, item, col, this) || '') : '';
      if (td.className !== nextClass) td.className = nextClass;
    });
  }
  _updateSummary(latest) {
    if (!this._summarySpan) return;
    let next = '';
    if (typeof this._summarySpec === 'string') {
      next = this._summarySpec;
    } else if (this._summarySpec && typeof this._summarySpec === 'object') {
      const fn = window._datatableFns?.[this._summarySpec.text];
      next = fn ? (fn(latest, this) || '') : '';
    }
    if (this._summarySpan.textContent !== String(next)) this._summarySpan.textContent = String(next);
  }
  async render() {
    if (this._busy || !this._src) return;
    this._busy = true;
    let data;
    try {
      const res = await fetch(this._src, {});
      if (!res.ok) {
        this.insertStatus(`Failed to fetch data (HTTP ${res.status}).`, 'error');
        this._busy = false;
        return;
      }
      data = await res.json();
    } catch (err) {
      console.error(err);
      this.insertStatus('Unable to load data (network or JSON parse error).', 'error');
      this._busy = false;
      return;
    }
    const rows = Array.isArray(data?.rows) ? data.rows : Array.isArray(data?.dhcptable) ? data.dhcptable : Array.isArray(data?.hwtable) ? data.hwtable : [];
    this._updateSummary(data);
    if (rows.length === 0) {
      if (this._tbody?.firstChild) this._tbody.replaceChildren();
      this._rowIndex.clear();
      this.insertStatus('No data to display.');
      this._busy = false;
      return;
    }
    const seen = new Set();
    for (let i = 0; i < rows.length; i++) {
      const item = rows[i];
      const key = this._getKey(item);
      let tr = this._rowIndex.get(key);
      if (!tr) {
        tr = this._createRow(item, key);
        this._rowIndex.set(key, tr);
      } else {
        this._updateRow(tr, item);
      }
      const atIndex = this._tbody.children[i];
      if (tr !== atIndex) this._tbody.insertBefore(tr, atIndex ?? null);
      seen.add(key);
    }
    for (const [key, tr] of Array.from(this._rowIndex.entries())) {
      if (!seen.has(key)) {
        tr.remove();
        this._rowIndex.delete(key);
      }
    }
    this._busy = false;
  }
}
customElements.define('data-table', DataTableElement);