// upgradebutton.js (Fetch-only, no metadata headers)
document.addEventListener('DOMContentLoaded', () => {
  const btn = document.getElementById('btn-upgrade');
  const statusEl = document.getElementById('status');
  if (!btn) return;

  const UPLOAD_URL = '/var/pico.bin'; // exact upload URL
  const UPLOAD_METHOD = 'POST'; // change to 'POST' if required
  const MAX_SIZE_BYTES = 32 * 1024 * 1024; // optional size guard (32 MiB)

  const setBusy = (busy, label) => {
    btn.disabled = busy;
    btn.textContent = label ?? (busy ? 'Uploading…' : 'Upgrade');
  };
  const setStatus = (text) => {
    statusEl.textContent = text || '';
  };

  btn.addEventListener('click', async () => {
    // Create a one-off hidden file input
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = 'application/octet-stream,.bin';
    input.style.display = 'none';
    document.body.appendChild(input);
    input.click();

    input.addEventListener('change', async () => {
      const file = input.files?.[0];
      input.remove(); // cleanup
      if (!file) return; // user cancelled

      // Basic validations
      if (!/\.bin$/i.test(file.name)) {
        alert('Please choose a .bin firmware file.');
        return;
      }
      if (file.size === 0) {
        alert('Selected file is empty.');
        return;
      }
      if (file.size > MAX_SIZE_BYTES) {
        alert(`File too large (${file.size} bytes). Max allowed is ${MAX_SIZE_BYTES} bytes.`);
        return;
      }

      setBusy(true, 'Uploading…');
      setStatus(`File: ${file.name} (${file.size} bytes)`);

      try {

        const controller = new AbortController();
        const timeout = setTimeout(() => controller.abort(), 30000); // abort after 30s if stuck

        const respUpload = await fetch(UPLOAD_URL, {
          method: UPLOAD_METHOD,
          // Let the browser set headers; fewer headers = simpler CORS preflight
          body: file,
          signal: controller.signal
        });

        clearTimeout(timeout);

        if (!respUpload.ok) {
          const text = await safeText(respUpload);
          throw new Error(`Upload failed: HTTP ${respUpload.status} ${respUpload.statusText}${text ? ` — ${text}` : ''}`);
        }

        setBusy(true, 'Applying…');
        setStatus('Upload complete. Sending Upgrade Command');
        // Send POST request to /reboot endpoint
        const respUpgrade = await fetch('/api/upgrade.json', {
          method: 'POST',
          headers: {
            'Ok': 'application/json'
          }
        });

        if (!respUpgrade.ok) {
          const text = await safeText(respUpgrade);
          throw new Error(`Upload failed: HTTP ${respUpgrade.status} ${respUpgrade.statusText}${text ? ` — ${text}` : ''}`);
        }
        setStatus('Upload complete. The device may be applying the update…');

        // Optional redirect after a short delay:
        setTimeout(() => window.location.replace('/index.html'), 5000);
      } catch (err) {
        console.error(err);
        alert(err?.message || 'Upload failed. Please check server logs and try again.');
      } finally {
        setBusy(false, 'Upgrade');
      }
    }, {
      once: true
    });
  });

  async function safeText(resp) {
    try {
      return await resp.text();
    } catch {
      return '';
    }
  }
});