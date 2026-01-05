// js/rebootbutton.js
document.addEventListener('DOMContentLoaded', function() {
  const btn = document.getElementById('btn-reboot');
  if (!btn) return;

  btn.addEventListener('click', async function() {
    // Show confirmation dialog before rebooting
    const confirmed = confirm(
      'Are you sure you want to reboot the device?\n\nActive sessions and operations may be interrupted.'
    );
    if (!confirmed) return;

    btn.disabled = true;
    btn.textContent = 'Rebooting...';

    try {
      // Send POST request to /reboot endpoint
      const resp = await fetch('/api/reboot.json', {
        method: 'POST',
        headers: {
          'Ok': 'application/json'
        }
      });

      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);

      let remaining = 5;
      const ticker = setInterval(() => {
        remaining--;
        if (remaining <= 0) clearInterval(ticker);
      }, 1000);
      setTimeout(() => {
        window.location.replace('/index.html');
      }, 5000); // redirect after 10s
    } catch (err) {
      alert('Failed to send reboot command. Please try again or check server logs.');
      btn.disabled = false;
      btn.textContent = 'Reboot';
    }
  });
});