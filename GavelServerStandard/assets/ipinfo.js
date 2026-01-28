// ip-info.js
const IpInfo = (() => {
  let cache = null;
  let inflight = null;

  async function fetchIpInfoOnce() {
    try {
      const resp = await fetch('/api/ip-info.json', {
        headers: {
          'Accept': 'application/json'
        }
      });
      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
      const json = await resp.json();
      return {
        macAddress: json.macAddress || 'unknown',
        ipAddress: json.ipAddress || 'unknown',
        subnetMask: json.subnetMask || 'unknown',
        gatewayAddress: json.gatewayAddress || 'unknown',
        dnsAddress: json.dnsAddress || 'unknown',
        isDHCP: typeof json.isDHCP === 'boolean' ? json.isDHCP : null,
        allowDHCP: typeof json.allowDHCP === 'boolean' ? json.allowDHCP : null,
        isWifi: typeof json.isWifi === 'boolean' ? json.isWifi : null
      };
    } catch (err) {
      return {
        macAddress: 'unknown',
        ipAddress: 'unknown',
        subnetMask: 'unknown',
        gatewayAddress: 'unknown',
        dnsAddress: 'unknown',
        isDHCP: true,
        allowDHCP: true,
        isWifi: false
      };
    } finally {
      inflight = null;
    }
  }

  async function getIpInfo() {
    if (cache) return cache;
    if (inflight) return inflight;
    inflight = (async () => {
      const result = await fetchIpInfoOnce();
      cache = result;
      return result;
    })();
    return inflight;
  }

  /**
   * Explicitly refreshes and replaces the cache with a fresh fetch.
   * If a fetch is already in-flight, this will begin a new one and
   * replace the shared `inflight` so concurrent callers coalesce.
   */
  async function refreshIpInfo() {
    const p = (async () => {
      const result = await fetchIpInfoOnce();
      cache = result;
      return result;
    })();
    inflight = p;
    return p;
  }

  /**
   * Optional: exposes a manual cache clear without refetching.
   */
  function clearIpInfoCache() {
    cache = null;
    // Leave inflight as-is; callers can still await the ongoing fetch
  }

  return {
    getIpInfo,
    refreshIpInfo,
    clearIpInfoCache
  };
})();

export {
  IpInfo
};