(async function () {
  try {
    const response = await fetch('/api/ip-info.json', { cache: 'no-store' });
    if (!response.ok) throw new Error('Failed to fetch IP info');

    const { ipAddress = '' } = await response.json();
    const ip = ipAddress.trim();

    const ipv4Pattern = /^(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}$/;

    if (ipv4Pattern.test(ip)) {
      document.getElementById('refresh-meta')
        ?.setAttribute('content', `3; url=http://${ip}`);
    } else {
      console.warn('Invalid or missing ipAddress in JSON');
    }
  } catch (error) {
    console.error('Redirect setup failed:', error);
  }
})();
