// /js/transfer.js
// Exposes downloadJSON and uploadJSON on the global window so inline onclick handlers work.

// -------- Download (simple link-trigger) --------
window.downloadJSON = function downloadJSON() {
  try {
    const url = '/api/export.json';
    const link = document.createElement('a');
    link.href = url;
    link.download = 'export.json'; // Suggested filename (browser may ignore)
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  } catch (err) {
    console.error('Download failed:', err);
    alert('An unexpected error occurred while starting the download.');
  }
};

// -------- Upload (validate + POST JSON) --------
window.uploadJSON = async function uploadJSON() {
  // Create a one-off invisible file input
  const input = document.createElement('input');
  input.type = 'file';
  input.accept = 'application/json,.json';
  input.style.display = 'none';
  document.body.appendChild(input);

  input.addEventListener('change', async () => {
    const file = input.files && input.files[0];
    // Clean up the input element early
    document.body.removeChild(input);

    if (!file) {
      // No file chosen
      return;
    }

    // Optional: size guard
    const MAX_BYTES = 5 * 1024 * 1024; // 5 MiB
    if (file.size > MAX_BYTES) {
      alert('Selected file is too large. Please choose a file under 5 MB.');
      return;
    }

    try {
      const text = await file.text();

      // Minimal validation: ensure it's valid JSON
      let payload;
      try {
        payload = JSON.parse(text);
      } catch (parseErr) {
        console.error('Invalid JSON:', parseErr);
        alert('The selected file is not valid JSON.');
        return;
      }

      // OPTIONAL: add schema checks here:
      // if (!payload || typeof payload !== 'object' || !payload.config) {
      //   alert('JSON is missing required "config" object.');
      //   return;
      // }

      // POST JSON to backend (adjust endpoint/method if needed)
      const resp = await fetch('/api/export.json', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload),
      });

      if (!resp.ok) {
        let serverMsg = '';
        try {
          const errJson = await resp.json();
          serverMsg = errJson?.message || JSON.stringify(errJson);
        } catch {
          serverMsg = await resp.text();
        }
        alert(`Import failed (${resp.status}). ${serverMsg || 'Please check the server logs.'}`);
        return;
      }

      // Optional: read server response
      let resultText = '';
      try {
        const result = await resp.json();
        resultText = result?.message || 'Import succeeded.';
      } catch {
        resultText = 'Import succeeded.';
      }

      alert(resultText);

      // Optional: refresh displayed info after successful import
      try {
        if (typeof BuildInfo !== 'undefined' && typeof IpInfo !== 'undefined') {
          await (async function repopulate() {
            try {
              const bi = await BuildInfo.getBuildInfo();
              setText('prog-product', safe(bi.product));
              setText('prog-version', safe(bi.version));
              setText('prog-date', safe(bi.build_date));
              setText('prog-time', safe(bi.build_time));
              setText('prog-author', safe(bi.author));
              setText('prog-device', safe(bi.device ?? '—'));

              const ii = await IpInfo.getIpInfo();
              setText('ip-mac', safe(ii.macAddress));
              setText('ip-addr', safe(ii.ipAddress));
              setText('ip-mask', safe(ii.subnetMask));
              setText('ip-gw', safe(ii.gatewayAddress));
              setText('ip-dns', safe(ii.dnsAddress));
              setText('cur-ip-addr', safe(ii.ipAddress));
              setCheckbox('dhcp', !!ii.isDHCP);
            } catch (err) {
              console.error(err);
            }
          })();
        }
      } catch (refreshErr) {
        console.warn('Refresh after import failed:', refreshErr);
      }
    } catch (err) {
      console.error('Upload failed:', err);
      alert('An unexpected error occurred while uploading the file.');
    }
  });

  // Trigger the file picker
  input.click();
};
