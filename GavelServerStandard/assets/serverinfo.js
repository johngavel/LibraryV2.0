// server-info.js
// Fetches build info once and caches it. Safe for embedded environments.
const ServerInfo = (() => {
  let cache = null;
  let inflight = null;

  // Single-shot fetch with cache-busting, returns a normalized object.
  async function fetchServerInfoOnce() {
    try {
      const url = `/api/server-info.json`; // bust HTTP caches
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
        programInfo: json.programInfo || false,
        ethernetInfo: json.ethernetInfo || false,
        memoryInfo: json.memoryInfo || false,
        licenseInfo: json.licenseInfo || false,
        hwInfo: json.hwInfo || false,
        rebootInfo: json.rebootInfo || false,
        upgradeInfo: json.upgradeInfo || false,
        uploadInfo: json.uploadInfo || false,
        terminalInfo: json.terminalInfo || false
      };
    } catch (err) {
      // Fallback if endpoint is unavailable
      return {
        programInfo: false,
        ethernetInfo: false,
        memoryInfo: false,
        licenseInfo: false,
        hwInfo: false,
        rebootInfo: false,
        upgradeInfo: false,
        uploadInfo: false,
        terminalInfo: false
      };
    }
  }

  async function getServerInfo() {
    if (cache) return cache;
    if (inflight) return inflight;

    inflight = (async () => {
      const result = await fetchServerInfoOnce();
      cache = result;
      inflight = null;
      return result;
    })();

    return inflight;
  }

  // Forces a fresh fetch and replaces the cache.
  async function refreshServerInfo() {
    const p = (async () => {
      const result = await fetchServerInfoOnce();
      cache = result;
      return result;
    })();
    inflight = p; // coalesce concurrent callers to the same refresh
    return p;
  }

  // Optional: clear the cache without refetching.
  function clearServerInfoCache() {
    cache = null;
    // leave inflight as-is; callers may still await an ongoing fetch
  }

  return {
    getServerInfo,
    refreshServerInfo,
    clearServerInfoCache
  };
})();

export {
  ServerInfo
};