document.addEventListener('DOMContentLoaded', () => {
  console.log('✔ Script loaded and DOM ready');
  const queryApiBase = new URLSearchParams(location.search).get('apiBase');
  const apiBase =
    queryApiBase ||
    (typeof window.CONSOLE_API_BASE === 'string' && window.CONSOLE_API_BASE) ||
    window.location.origin;
  const ENDPOINTS = {
    events: `${apiBase.replace(/\/+$/, '')}/api/sse_events`,
    command: `${apiBase.replace(/\/+$/, '')}/api/sse_command`
  };
  const csrfToken = document.querySelector('meta[name="csrf-token"]')?.content;
  const terminal = document.getElementById('terminal');
  const input = document.getElementById('input');
  const status = document.getElementById('status');
  let eventSource = null;
  let lastHeartbeat = Date.now();
  const HEARTBEAT_TIMEOUT_MS = 30_000;
  let reconnectBackoffMs = 5000;
  const RECONNECT_BACKOFF_MAX_MS = 60_000;
  const commandHistory = [];
  let historyIndex = 0;
  function updateStatus(text, level = 'ok') {
    status.textContent = text;
    status.classList.remove('ok', 'warn', 'error');
    if (level === 'ok') status.classList.add('ok');
    else if (level === 'error') status.classList.add('error');
    else status.classList.add('warn');
  }
  const ANSI_FG_COLORS = {
    30: 'black',
    31: 'red',
    32: 'limegreen',
    33: 'yellow',
    34: 'dodgerblue',
    35: 'magenta',
    36: 'cyan',
    37: 'white'
  };
  function parseANSI(text) {
    const frag = document.createDocumentFragment();
    const ansiRe = /\x1b\[([\d;]*)([A-Za-z])/g;
    let lastIndex = 0;
    let match;
    let currentColor = '';
    let currentBold = false;
    let currentUnderline = false;
    function pushChunk(chunk) {
      if (!chunk) return;
      const span = document.createElement('span');
      span.textContent = chunk;
      if (currentColor) span.style.color = currentColor;
      if (currentBold) span.style.fontWeight = 'bold';
      if (currentUnderline) span.style.textDecoration = 'underline';
      frag.appendChild(span);
    }
    while ((match = ansiRe.exec(text)) !== null) {
      const chunk = text.slice(lastIndex, match.index);
      pushChunk(chunk);
      const params = match[1] ? match[1].split(';').map(Number) : [];
      const command = match[2];
      if (command === 'm') {
        if (params.length === 0) {
          currentColor = '';
          currentBold = false;
          currentUnderline = false;
        } else {
          for (const code of params) {
            if (code === 0) {
              currentColor = '';
              currentBold = false;
              currentUnderline = false;
            } else if (code === 1) {
              currentBold = true;
            } else if (code === 4) {
              currentUnderline = true;
            } else if (ANSI_FG_COLORS[code]) {
              currentColor = ANSI_FG_COLORS[code];
            }
          }
        }
      } else if (command === 'J') {
        const p = params[0];
        if (p === 2 || params.length === 0) {
          terminal.innerHTML = '';
        }
      } else if (command === 'H') {
        terminal.scrollTop = 0;
      }
      lastIndex = ansiRe.lastIndex;
    }
    pushChunk(text.slice(lastIndex));
    return frag;
  }
  function appendMessage(text, cssClass = '') {
    const message = document.createElement('div');
    if (cssClass) message.className = cssClass;
    message.appendChild(parseANSI(text));
    terminal.appendChild(message);
    while (terminal.children.length > 500) {
      terminal.removeChild(terminal.firstChild);
    }
    terminal.scrollTop = terminal.scrollHeight;
  }
  function appendCommand(text) {
    let lastChild = terminal.lastElementChild;
    if (!lastChild) {
      lastChild = document.createElement('div');
      terminal.appendChild(lastChild);
    }
    lastChild.appendChild(parseANSI(text));
    terminal.scrollTop = terminal.scrollHeight;
  }
  function scheduleReconnect() {
    updateStatus(`Connection lost. Reconnecting in ${Math.round(reconnectBackoffMs / 1000)}s…`, 'error');
    setTimeout(connectSSE, reconnectBackoffMs);
    reconnectBackoffMs = Math.min(reconnectBackoffMs * 2, RECONNECT_BACKOFF_MAX_MS);
  }
  function connectSSE() {
    try {
      if (eventSource) {
        try { eventSource.close(); } catch {}
        eventSource = null;
      }
      updateStatus('Connecting…', 'warn');
      eventSource = new EventSource(ENDPOINTS.events, { withCredentials: true });
      eventSource.onopen = () => {
        updateStatus('Connected', 'ok');
        lastHeartbeat = Date.now();
        reconnectBackoffMs = 2000;
      };
      eventSource.onmessage = (event) => {
        appendMessage(event.data);
        lastHeartbeat = Date.now();
      };
      eventSource.addEventListener('heartbeat', () => {
        lastHeartbeat = Date.now();
      });
      eventSource.onerror = () => {
        appendMessage('[Error] Connection error detected. Attempting to reconnect…', 'error');
        try { eventSource && eventSource.close(); } catch {}
        scheduleReconnect();
      };
    } catch (e) {
      appendMessage(`[Error] ${e?.message || e}`, 'error');
      scheduleReconnect();
    }
  }
  setInterval(() => {
    if (Date.now() - lastHeartbeat > HEARTBEAT_TIMEOUT_MS) {
      updateStatus('No heartbeat. Reconnecting…', 'error');
      appendMessage('[Warning] No heartbeat detected. Reconnecting…', 'warn');
      try { eventSource && eventSource.close(); } catch {}
      scheduleReconnect();
    }
  }, 10_000);
  function submitCommand(cmd) {
    const command = cmd.trim();
    if (!command) return;
    appendCommand(command);
    commandHistory.push(command);
    historyIndex = commandHistory.length;
    const headers = { 'Content-Type': 'application/json' };
    if (csrfToken) headers['X-CSRF-Token'] = csrfToken;
    fetch(ENDPOINTS.command, {
      method: 'POST',
      headers,
      credentials: 'same-origin',
      body: JSON.stringify({ command })
    }).then((response) => {
      if (!response.ok) {
        appendMessage(`[Error] Server responded with ${response.status}`, 'error');
      }
    }).catch((error) => {
      appendMessage(`[Error] ${error.message}`, 'error');
    });
  }
  input.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
      submitCommand(input.value);
      input.value = '';
      return;
    }
    if (e.key === 'ArrowUp') {
      if (historyIndex > 0) {
        historyIndex--;
        input.value = commandHistory[historyIndex] || '';
      }
      e.preventDefault();
      return;
    }
    if (e.key === 'ArrowDown') {
      if (historyIndex < commandHistory.length - 1) {
        historyIndex++;
        input.value = commandHistory[historyIndex] || '';
      } else {
        historyIndex = commandHistory.length;
        input.value = '';
      }
      e.preventDefault();
      return;
    }
  });
  connectSSE();
});