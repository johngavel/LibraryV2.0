// footer.js
(async () => {
  const hostEl = document.getElementById('site-footer');
  if (!hostEl) return;

  const info = await BuildInfo.getBuildInfo();

  hostEl.innerHTML = `
    <div class="footer-content">
      <div>${escapeHtml(info.product)}</div>
      <div>Ver. ${escapeHtml(info.version)}</div>
      <div>Build Date: ${escapeHtml(info.build_date)} Time: ${escapeHtml(info.build_time)}</div>
      <div>Author: ${escapeHtml(info.author)}</div>
    </div>
  `;
})();

function escapeHtml(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}