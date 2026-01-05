// /js/libraryTable.js
(async function renderLibraryTableFromAPI() {
  // Grab existing containers defined in template.html
  const mainTitle = document.getElementById('library-count');
  const tablediv = document.getElementById('library-table');
  if (!tablediv) return; // Quiet exit if structure isn't present

  // Utilities: status messages that use your CSS classes
  function insertStatus(message, type = 'warn') {
    const p = document.createElement('p');
    p.className = type; // 'warn' or 'error' (matches style.css)
    p.textContent = message;
    tablediv.appendChild(p);
  }

  let data;
  try {
    const res = await fetch('/api/license-info.json', {});

    if (!res.ok) {
      insertStatus(`Failed to fetch license info (HTTP ${res.status}).`, 'error');
      return;
    }
    data = await res.json();
  } catch (err) {
    console.error(err);
    insertStatus('Unable to load license info (network or JSON parse error).', 'error');
    return;
  }

  // Validate input
  const libs = Array.isArray(data?.library) ? data.library : [];
  const count = Number.isFinite(data?.countLibrary) ? data.countLibrary : libs.length;

  // Set the main title (uses your h2 styles)
  if (mainTitle) {
    mainTitle.textContent = `Libraries (${count})`;
  }

  if (libs.length === 0) {
    insertStatus('No libraries to display.');
    return;
  }

  // Build table (unstyled class-free; uses your global table/td rules)
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

  // Insert into existing page
  tablediv.appendChild(table);
})();