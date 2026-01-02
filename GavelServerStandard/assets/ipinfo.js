// ip-info.js
// Fetches build info once and caches it. Safe for embedded environments.
const IpInfo = (() => {
  let cache = null;
  let inflight = null;

  async function getIpInfo() {
    if (cache) return cache;
    if (inflight) return inflight;

    inflight = (async () => {
      try {
        const resp = await fetch('/api/ip-info.json', { headers: { 'Accept': 'application/json' } });
        if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
        const json = await resp.json();

        // Normalize keys (accept multiple casings)
        cache = {
          macAddress: json.macAddress || 'unknown',
          ipAddress: json.ipAddress || 'unknown',
          subnetMask: json.subnetMask || 'unknown',
          gatewayAddress: json.gatewayAddress || 'unknown',
          dnsAddress: json.dnsAddress || 'unknown',
          isDHCP: typeof json.isDHCP === 'boolean' ? json.isDHCP : null,
          allowDHCP: typeof json.allowDHCP === 'boolean' ? json.allowDHCP : null
        };
      } catch (err) {
        // Fallback if endpoint is unavailable
        cache = {
          macAddress: 'unknown',
          ipAddress: 'unknown',
          subnetMask: 'unknown',
          gatewayAddress: 'unknown',
          dnsAddress: 'unknown',
          isDHCP: null,
          allowDHCP: null
        };
      }
      inflight = null;
      return cache;
    })();

    return inflight;
  }

  return { getIpInfo };
})();
