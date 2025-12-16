// build-info.js
// Fetches build info once and caches it. Safe for embedded environments.
const BuildInfo = (() => {
  let cache = null;
  let inflight = null;

  async function getBuildInfo() {
    if (cache) return cache;
    if (inflight) return inflight;

    inflight = (async () => {
      try {
        const resp = await fetch('/api/build-info.json', { headers: { 'Accept': 'application/json' } });
        if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
        const json = await resp.json();

        // Normalize keys (accept multiple casings)
        cache = {
          product: json.product || json.name || 'unknown',
          version: json.version || json.ver || 'unknown',
          build_date: json.build_date || json.date || 'unknown',
          build_time: json.build_time || json.time || 'unknown',
          author: json.author || json.built_by || 'unknown',
          device: json.device || json.board || undefined,
          firmware: json.firmware || json.fw || undefined
        };
      } catch (err) {
        // Fallback if endpoint is unavailable
        cache = {
          product: 'unknown',
          version: 'unknown',
          build_date: 'unknown',
          build_time: 'unknown',
          author: 'unknown'
        };
      }
      inflight = null;
      return cache;
    })();

    return inflight;
  }

  return { getBuildInfo };
})();
