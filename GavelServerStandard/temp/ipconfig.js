// ipconfig.js (ES module)
import {
  IpInfo
} from '/js/ipinfo.js';

// ---------- Validators ----------
const isIPv4 = (s) => {
  if (typeof s !== 'string') return false;
  if (!/^\d{1,3}(\.\d{1,3}){3}$/.test(s)) return false;
  return s.split('.').every(n => {
    const v = +n;
    return Number.isInteger(v) && v >= 0 && v <= 255;
  });
};

const VALID_MASKS = new Set([
  '255.0.0.0', '255.128.0.0', '255.192.0.0', '255.224.0.0', '255.240.0.0', '255.248.0.0', '255.252.0.0', '255.254.0.0',
  '255.255.0.0', '255.255.128.0', '255.255.192.0', '255.255.224.0', '255.255.240.0', '255.255.248.0', '255.255.252.0', '255.255.254.0',
  '255.255.255.0', '255.255.255.128', '255.255.255.192', '255.255.255.224', '255.255.255.240', '255.255.255.248', '255.255.255.252', '255.255.255.254', '255.255.255.255'
]);
const isMask = (s) => isIPv4(s) && VALID_MASKS.has(s);

// ---------- UI helpers ----------
function setStaticInputsEnabled(enabled, ipAddress, subnetMask, gatewayAddress, dnsAddress) {
  [ipAddress, subnetMask, gatewayAddress, dnsAddress].forEach(i => {
    i.disabled = !enabled;
    i.required = enabled;
  });
}

function showMsg(kind, text, msgEl) {
  // kind: "", "ok", "err"
  msgEl.className = `msg ${kind}`.trim();
  msgEl.textContent = text ?? '';
  msgEl.style.display = (text ?? kind) ? 'block' : 'none';
}

// ---------- Load current info ----------
async function load(IpInfo, els) {
  const {
    macAddress,
    currentIp,
    allowdhcp,
    dhcp,
    ipAddress,
    subnetMask,
    gatewayAddress,
    dnsAddress,
    msg
  } = els;

  try {
    showMsg('', '', msg);

    const info = await IpInfo.getIpInfo();

    // Display-only fields are <div> elements, use textContent
    macAddress.textContent = info.macAddress ?? 'unknown';
    currentIp.textContent = info.ipAddress ?? 'unknown';

    let dhcpMode = !!info.isDHCP;
    const showDHCP = !!info.allowDHCP;

    if (showDHCP === false) {
      allowdhcp.classList.add('hidden');
      dhcpMode = false;
    } else {
      allowdhcp.classList.remove('hidden');
    }

    dhcp.checked = dhcpMode;

    ipAddress.value = (info.ipAddress && info.ipAddress !== 'unknown') ? info.ipAddress : '';
    subnetMask.value = (info.subnetMask && info.subnetMask !== 'unknown') ? info.subnetMask : '';
    gatewayAddress.value = (info.gatewayAddress && info.gatewayAddress !== 'unknown') ? info.gatewayAddress : '';
    dnsAddress.value = (info.dnsAddress && info.dnsAddress !== 'unknown') ? info.dnsAddress : '';

    setStaticInputsEnabled(!dhcpMode, ipAddress, subnetMask, gatewayAddress, dnsAddress);
  } catch (err) {
    showMsg('err', `Could not load current IP settings: ${err.message ?? err}`, msg);
  }
}

// ---------- Validation & payload ----------
function validateAndBuildPayload(dhcp, ipAddress, subnetMask, gatewayAddress, dnsAddress) {
  const dhcpMode = dhcp.checked;

  if (!dhcpMode) {
    if (!isIPv4(ipAddress.value)) return {
      error: 'Please enter a valid IPv4 address.'
    };
    if (!isMask(subnetMask.value)) return {
      error: 'Please enter a valid subnet mask.'
    };
    if (gatewayAddress.value && !isIPv4(gatewayAddress.value)) return {
      error: 'Gateway must be a valid IPv4 address.'
    };
    if (dnsAddress.value && !isIPv4(dnsAddress.value)) return {
      error: 'DNS must be a valid IPv4 address.'
    };
  }

  return {
    payload: {
      isDHCP: dhcpMode,
      ipAddress: dhcpMode ? null : ipAddress.value.trim(),
      subnetMask: dhcpMode ? null : subnetMask.value.trim(),
      gatewayAddress: dhcpMode ? null : gatewayAddress.value.trim(),
      dnsAddress: dhcpMode ? null : dnsAddress.value.trim(),
    }
  };
}

// ---------- Save settings ----------
async function saveSettings(payload) {
  const resp = await fetch('/api/ip-info.json', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
    },
    body: JSON.stringify(payload),
  });
  if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
  return resp.json().catch(() => ({}));
}

// ---------- Reboot device ----------
async function rebootDevice() {
  try {
    const resp = await fetch('/api/reboot.json', {
      method: 'POST',
      headers: {
        'Accept': 'application/json'
      }
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
  } catch (err) {
    alert('Failed to send reboot command.');
  }
}

// ---------- Public init ----------
export async function initIpConfig() {
  const $ = (id) => document.getElementById(id);

  const allowdhcp = $('allowdhcp');
  const dhcp = $('dhcp');
  const ipAddress = $('ipAddress');
  const subnetMask = $('subnetMask');
  const gatewayAddress = $('gatewayAddress');
  const dnsAddress = $('dnsAddress');

  const macAddress = $('macAddress'); // <div>
  const currentIp = $('currentIp'); // <div>

  const msg = $('msg');
  const form = $('ipForm');
  const btnReload = $('btnReload');
  const btnSaveReboot = $('btnSaveReboot');

  dhcp?.addEventListener('change', () =>
    setStaticInputsEnabled(!dhcp.checked, ipAddress, subnetMask, gatewayAddress, dnsAddress)
  );

  function reload() {
    IpInfo.refreshIpInfo();
    load(IpInfo, {
      macAddress,
      currentIp,
      allowdhcp,
      dhcp,
      ipAddress,
      subnetMask,
      gatewayAddress,
      dnsAddress,
      msg
    });
  }
  btnReload?.addEventListener('click', reload);

  form?.addEventListener('submit', async (ev) => {
    ev.preventDefault();
    showMsg('', '', msg);

    const check = validateAndBuildPayload(dhcp, ipAddress, subnetMask, gatewayAddress, dnsAddress);
    if (check.error) return showMsg('err', check.error, msg);

    try {
      const result = await saveSettings(check.payload);
      showMsg('ok', result.message ?? 'Network settings updated successfully.', msg);
    } catch (err) {
      showMsg('err', `Failed to save settings: ${err.message ?? err}`, msg);
    }
  });

  btnSaveReboot?.addEventListener('click', async () => {
    showMsg('', '', msg);

    const check = validateAndBuildPayload(dhcp, ipAddress, subnetMask, gatewayAddress, dnsAddress);
    if (check.error) return showMsg('err', check.error, msg);

    const confirmed = confirm(
      'Are you sure you want to reboot the device?\n\nActive sessions and operations may be interrupted.'
    );
    if (!confirmed) return;

    try {
      await saveSettings(check.payload);
      showMsg('ok', 'Settings saved. Rebooting device…', msg);
      await rebootDevice();
      showMsg('ok', 'Save & Reboot initiated. The device may be temporarily unavailable.', msg);
      setTimeout(() => window.location.replace('/index.html'), 5000);
    } catch (err) {
      showMsg('err', err.message ?? String(err), msg);
    }
  });

  await load(IpInfo, {
    macAddress,
    currentIp,
    allowdhcp,
    dhcp,
    ipAddress,
    subnetMask,
    gatewayAddress,
    dnsAddress,
    msg
  });
}

window.addEventListener('DOMContentLoaded', () => {
  initIpConfig();
});