// header.js
(async () => {
  const hostEl = document.getElementById('site-header');
  if (!hostEl) return;

  const info = await BuildInfo.getBuildInfo();
  document.title = `${info.product}`

  hostEl.innerHTML = `
    <h1>${escapeHtml(info.product)}</h1>
    <hr style="width:50%">
  `;
})();

// Minimal HTML-escape to avoid issues if API returns special characters
function escapeHtml(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}