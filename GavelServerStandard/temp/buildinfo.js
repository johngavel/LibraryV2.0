// build-info.js
// Fetches build info once and caches it. Safe for embedded environments.
const BuildInfo = (() => {
  let cache = null;
  let inflight = null;

  // Single-shot fetch with cache-busting, returns a normalized object.
  async function fetchBuildInfoOnce() {
    try {
      const url = `/api/build-info.json?ts=${Date.now()}`; // bust HTTP caches
      const resp = await fetch(url, {
        headers: {
          'Accept': 'application/json',
          'Cache-Control': 'no-cache'
        },
        cache: 'no-store' // avoid browser cache
      });
      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
      const json = await resp.json();
      return {
        // Normalize keys (accept multiple casings)
        product: json.product || json.name || 'unknown',
        version: json.version || json.ver || 'unknown',
        build_date: json.build_date || json.date || 'unknown',
        build_time: json.build_time || json.time || 'unknown',
        author: json.author || json.built_by || 'unknown',
        device: json.device || json.board || undefined
      };
    } catch (err) {
      // Fallback if endpoint is unavailable
      return {
        product: 'unknown',
        version: 'unknown',
        build_date: 'unknown',
        build_time: 'unknown',
        author: 'unknown'
      };
    }
  }

  async function getBuildInfo() {
    if (cache) return cache;
    if (inflight) return inflight;

    inflight = (async () => {
      const result = await fetchBuildInfoOnce();
      cache = result;
      inflight = null;
      return result;
    })();

    return inflight;
  }

  // Forces a fresh fetch and replaces the cache.
  async function refreshBuildInfo() {
    const p = (async () => {
      const result = await fetchBuildInfoOnce();
      cache = result;
      return result;
    })();
    inflight = p; // coalesce concurrent callers to the same refresh
    return p;
  }

  // Optional: clear the cache without refetching.
  function clearBuildInfoCache() {
    cache = null;
    // leave inflight as-is; callers may still await an ongoing fetch
  }

  return {
    getBuildInfo,
    refreshBuildInfo,
    clearBuildInfoCache
  };
})();

export {
  BuildInfo
};