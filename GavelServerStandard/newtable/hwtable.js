// hwtable.js (wrapper)
// Thin wrapper that instantiates <data-table> with Hardware-specific config

class HardwareTableWrapper extends HTMLElement {
  connectedCallback() {
    const el = document.createElement('data-table');
    el.setAttribute('src', this.getAttribute('src') || '/api/hw-info.json');
    el.setAttribute('interval-ms', this.getAttribute('interval-ms') || '5000');
    el.setAttribute('row-key', this.getAttribute('row-key') || 'auto');
    el.setAttribute('summary', JSON.stringify({
      text: 'hwSummary'
    }));
    el.setAttribute('columns', JSON.stringify([{
      header: 'Name',
      field: 'hwname'
    }, {
      header: 'ID',
      field: 'hwid'
    }, {
      header: 'Status',
      field: 'hwstatus',
      text: 'hwOnlineText',
      classFor: 'hwOnlineClass'
    }]));
    this.replaceChildren(el);
  }
}
customElements.define('hardware-table', HardwareTableWrapper);

// Hardware-specific function registry kept local to this file
window._datatableFns = Object.assign(window._datatableFns || {}, {
  hwOnlineText: (v) => (v ? 'Online' : 'Offline'),
  hwOnlineClass: (v) => (v ? 'ok' : 'error'),
  hwSummary: (data) => {
    const list = Array.isArray(data?.hwtable) ? data.hwtable : [];
    const count = Number.isFinite(Number(data?.numberhw)) ? Number(data.numberhw) : list.length;
    return `Hardware Count: ${count}`;
  }
});