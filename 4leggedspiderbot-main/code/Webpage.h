#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"HTMLDELIM(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Spider Leg Tuner</title>
<style>
  :root {
    --bg: #14171a;
    --panel: #1e2226;
    --border: #2c3138;
    --text: #e7e9ea;
    --muted: #8b9198;
    --accent: #4fd1c5;
    --danger: #e06c75;
    --success: #48bb78;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    background: var(--bg);
    color: var(--text);
    padding: 16px;
    max-width: 480px;
  }
  h1 { font-size: 1.3rem; margin: 0 0 4px; }
  .status { font-size: 0.85rem; color: var(--muted); margin-bottom: 16px; }
  .status.connected { color: var(--accent); }
  .status.disconnected { color: var(--danger); }

  .group {
    background: var(--panel);
    border: 1px solid var(--border);
    border-radius: 10px;
    padding: 14px 16px;
    margin-bottom: 14px;
  }
  .group h2 {
    font-size: 0.95rem;
    margin: 0 0 10px;
    color: var(--accent);
    text-transform: uppercase;
    letter-spacing: 0.04em;
  }

  .state-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 10px;
  }
  .state-btn {
    background: #111417;
    border: 1px solid var(--border);
    color: var(--text);
    border-radius: 8px;
    padding: 10px;
    font-size: 0.85rem;
    font-weight: 600;
    cursor: pointer;
    text-transform: uppercase;
    transition: all 0.2s;
  }
  .state-btn:active { opacity: 0.7; }
  .state-btn.active {
    background: var(--accent);
    color: #0b0d0e;
    border-color: var(--accent);
  }

  .row { display: flex; align-items: center; gap: 10px; margin-bottom: 10px; }
  .row:last-child { margin-bottom: 0; }
  .row label { flex: 0 0 110px; font-size: 0.85rem; color: var(--muted); }
  .row input[type="range"] { flex: 1 1 auto; accent-color: var(--accent); }
  .row input[type="number"] {
    flex: 0 0 78px;
    background: #111417;
    border: 1px solid var(--border);
    color: var(--text);
    border-radius: 6px;
    padding: 5px 6px;
    font-size: 0.85rem;
    text-align: right;
  }
  .row input[type="number"]:focus { outline: none; border-color: var(--accent); }

  .row select {
    flex: 1 1 auto;
    background: #111417;
    border: 1px solid var(--border);
    color: var(--text);
    border-radius: 6px;
    padding: 7px 8px;
    font-size: 0.9rem;
  }
  .row select:focus { outline: none; border-color: var(--accent); }

  .action-btn {
    background: #111417;
    border: 1px solid var(--border);
    color: var(--text);
    border-radius: 6px;
    padding: 6px 16px;
    font-size: 0.85rem;
    font-weight: 600;
    cursor: pointer;
    text-transform: uppercase;
    transition: all 0.2s;
  }
  .action-btn:active { opacity: 0.7; }
  .action-btn.on {
    background: var(--danger);
    color: #0b0d0e;
    border-color: var(--danger);
  }
  .action-btn.off {
    background: var(--accent);
    color: #0b0d0e;
    border-color: var(--accent);
  }
  
  .master-ctrl-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 10px;
    margin-top: 6px;
  }
  
  .danger-btn {
    width: 100%;
    background: #4a2025;
    color: var(--text);
    border: 1px solid var(--danger);
    border-radius: 8px;
    padding: 8px;
    font-size: 0.8rem;
    font-weight: 600;
    cursor: pointer;
    text-transform: uppercase;
    transition: all 0.2s;
  }
  .danger-btn:active { opacity: 0.7; }

  .success-btn {
    width: 100%;
    background: #1b3a2a;
    color: var(--text);
    border: 1px solid var(--success);
    border-radius: 8px;
    padding: 8px;
    font-size: 0.8rem;
    font-weight: 600;
    cursor: pointer;
    text-transform: uppercase;
    transition: all 0.2s;
  }
  .success-btn:active { opacity: 0.7; }

  .triplet { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; margin-bottom: 4px; }
  .triplet .sub label { font-size: 0.78rem; color: var(--muted); display: block; margin-bottom: 4px; }
  .triplet .sub input[type="number"] {
    width: 100%;
    background: #111417;
    border: 1px solid var(--border);
    color: var(--text);
    border-radius: 6px;
    padding: 6px;
    font-size: 0.85rem;
    text-align: center;
  }

  footer { text-align: center; color: var(--muted); font-size: 0.75rem; margin-top: 18px; }

  .save-btn {
    width: 100%;
    background: var(--accent);
    color: #0b0d0e;
    border: none;
    border-radius: 10px;
    padding: 12px;
    font-size: 0.95rem;
    font-weight: 600;
    cursor: pointer;
    margin-bottom: 10px;
  }
  .save-btn:active { opacity: 0.8; }

  .reset-btn {
    width: 100%;
    background: #4a2025;
    color: var(--text);
    border: 1px solid var(--danger);
    border-radius: 10px;
    padding: 12px;
    font-size: 0.95rem;
    font-weight: 600;
    cursor: pointer;
    margin-bottom: 14px;
  }
  .reset-btn:active { opacity: 0.8; }
</style>
</head>
<body>

<h1>Spider Leg Tuner</h1>
<div class="status disconnected" id="status">Connecting...</div>

<div class="group">
  <h2>Robot Motion State</h2>
  <div class="state-grid">
    <button class="state-btn active" id="btn_sit" onclick="changeState('sit')">Sit</button>
    <button class="state-btn" id="btn_stand" onclick="changeState('stand')">Stand</button>
    <button class="state-btn" id="btn_fwd" onclick="changeState('forward')">Forward</button>
    <button class="state-btn" id="btn_rev" onclick="changeState('reverse')">Reverse</button>
  </div>
</div>

<div class="group">
  <h2>Body / Global</h2>
  <div class="row">
    <label>Height</label>
    <input type="range" id="bodyHeight_slider" min="0" max="80" step="1" value="50">
    <input type="number" id="bodyHeight_num" min="0" max="80" step="1" value="50">
  </div>
  <div class="row">
    <label>Width</label>
    <input type="range" id="bodyWidth_slider" min="0" max="50" step="1" value="30">
    <input type="number" id="bodyWidth_num" min="0" max="50" step="1" value="30">
  </div>
  <div class="row">
    <label>Stride</label>
    <input type="range" id="bodyStride_slider" min="0" max="80" step="1" value="40">
    <input type="number" id="bodyStride_num" min="0" max="80" step="1" value="40">
  </div>
  <div class="row">
    <label>Pitch (fwd/back)</label>
    <input type="range" id="bodyPitch_slider" min="-20" max="20" step="1" value="0">
    <input type="number" id="bodyPitch_num" min="-20" max="20" step="1" value="0">
  </div>
  <div class="row">
    <label>Roll (left/right)</label>
    <input type="range" id="bodyRoll_slider" min="-45" max="45" step="1" value="0">
    <input type="number" id="bodyRoll_num" min="-45" max="45" step="1" value="0">
  </div>
</div>

<div class="group" id="legSelectGroup">
  <h2>Leg Select &amp; Power</h2>
  <div class="row">
    <label>Active Leg</label>
    <select id="leg_select">
      <option value="0">Leg 0</option>
      <option value="1">Leg 1</option>
      <option value="2">Leg 2</option>
      <option value="3">Leg 3</option>
    </select>
  </div>
  <div class="row" style="margin-top: 10px;">
    <label>Leg Power</label>
    <button class="action-btn on" id="leg_power_btn" onclick="toggleLegPower()">POWER OFF</button>
  </div>
  <div class="row" style="margin-top: 6px;">
    <label>All Legs Master</label>
    <div class="master-ctrl-grid">
      <button class="success-btn" onclick="turnOnAllLegs()">Power ON All</button>
      <button class="danger-btn" onclick="turnOffAllLegs()">Power OFF All</button>
    </div>
  </div>
</div>

<div class="group">
  <h2>Geometry</h2>
  <div class="row">
    <label>W (width)</label>
    <input type="range" id="W_slider" min="0" max="40" step="1" value="0">
    <input type="number" id="W_num" min="0" max="40" step="1" value="0">
  </div>
  <div class="row">
    <label>H (height)</label>
    <input type="range" id="H_slider" min="0" max="40" step="1" value="0">
    <input type="number" id="H_num" min="0" max="40" step="1" value="0">
  </div>
  <div class="row">
    <label>L (lift)</label>
    <input type="range" id="L_slider" min="0" max="40" step="1" value="20">
    <input type="number" id="L_num" min="0" max="40" step="1" value="20">
  </div>
</div>

<div class="group">
  <h2>Servo Offsets (deg)</h2>
  <div class="row">
    <label>offC</label>
    <input type="range" id="offC_slider" min="0" max="180" step="1" value="90">
    <input type="number" id="offC_num" min="0" max="180" step="1" value="90">
  </div>
  <div class="row">
    <label>offF</label>
    <input type="range" id="offF_slider" min="0" max="180" step="1" value="90">
    <input type="number" id="offF_num" min="0" max="180" step="1" value="90">
  </div>
  <div class="row">
    <label>offT</label>
    <input type="range" id="offT_slider" min="0" max="180" step="1" value="90">
    <input type="number" id="offT_num" min="0" max="180" step="1" value="90">
  </div>
</div>

<div class="group">
  <h2>Servo Angle Control (deg)</h2>
  <div class="row">
    <label>C (coxa)</label>
    <input type="range" id="servoC_slider" min="0" max="180" step="1" value="90">
    <input type="number" id="servoC_num" min="0" max="180" step="1" value="90">
  </div>
  <div class="row">
    <label>F (femur)</label>
    <input type="range" id="servoF_slider" min="0" max="180" step="1" value="90">
    <input type="number" id="servoF_num" min="0" max="180" step="1" value="90">
  </div>
  <div class="row">
    <label>T (tibia)</label>
    <input type="range" id="servoT_slider" min="0" max="180" step="1" value="90">
    <input type="number" id="servoT_num" min="0" max="180" step="1" value="90">
  </div>
</div>

<div class="group">
  <h2>Servo Min / Max (deg)</h2>
  <div class="triplet">
    <div class="sub"><label>C min</label><input type="number" id="cMin_num" min="0" max="180" step="1" value="0"></div>
    <div class="sub"><label>C max</label><input type="number" id="cMax_num" min="0" max="180" step="1" value="180"></div>
    <div></div>
  </div>
  <div class="triplet">
    <div class="sub"><label>F min</label><input type="number" id="fMin_num" min="0" max="180" step="1" value="0"></div>
    <div class="sub"><label>F max</label><input type="number" id="fMax_num" min="0" max="180" step="1" value="180"></div>
    <div></div>
  </div>
  <div class="triplet">
    <div class="sub"><label>T min</label><input type="number" id="tMin_num" min="0" max="180" step="1" value="0"></div>
    <div class="sub"><label>T max</label><input type="number" id="tMax_num" min="0" max="180" step="1" value="180"></div>
    <div></div>
  </div>
</div>

<button class="save-btn" onclick="send('save')">Save to Flash</button>
<button class="reset-btn" onclick="send('reset')">Reload/Reset Defaults</button>

<footer>ESP32 Spider Leg Tuner &mdash; sends live over WebSocket.</footer>

<script>
let ws;
let activeLeg = 0;
const statusEl = document.getElementById("status");

const stateButtons = {
  'sit': document.getElementById("btn_sit"),
  'stand': document.getElementById("btn_stand"),
  'forward': document.getElementById("btn_fwd"),
  'reverse': document.getElementById("btn_rev")
};

function connect() {
  ws = new WebSocket("ws://" + location.host + "/ws");
  ws.onopen = () => {
    statusEl.textContent = "Connected";
    statusEl.className = "status connected";
  };
  ws.onclose = () => {
    statusEl.textContent = "Disconnected - retrying...";
    statusEl.className = "status disconnected";
    setTimeout(connect, 1000);
  };
  ws.onerror = () => { ws.close(); };

  ws.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data);

      updateControl("bodyHeight", data.bH);
      updateControl("bodyWidth", data.bW);
      updateControl("bodyStride", data.bS);
      updateControl("bodyPitch", data.bP);
      updateControl("bodyRoll", data.bR);

      document.getElementById("leg_select").value = data.aL;
      activeLeg = data.aL;

      updateStateButtons(data.cSt);

      if (data.leg !== undefined) {
        updateControl("W", data.leg.W);
        updateControl("H", data.leg.H);
        updateControl("L", data.leg.L);
        updateControl("offC", data.leg.offC);
        updateControl("offF", data.leg.offF);
        updateControl("offT", data.leg.offT);
        updateControl("servoC", data.leg.servoC);
        updateControl("servoF", data.leg.servoF);
        updateControl("servoT", data.leg.servoT);

        document.getElementById("cMin_num").value = data.leg.cMin;
        document.getElementById("cMax_num").value = data.leg.cMax;
        document.getElementById("fMin_num").value = data.leg.fMin;
        document.getElementById("fMax_num").value = data.leg.fMax;
        document.getElementById("tMin_num").value = data.leg.tMin;
        document.getElementById("tMax_num").value = data.leg.tMax;

        if (data.leg.enabled !== undefined) {
          setLegPowerUI(data.leg.enabled);
        }
      }
    } catch(e) {
      console.error("Failed parsing incoming initialization frame: ", e);
    }
  };
}
connect();

function updateStateButtons(stateStr) {
  Object.keys(stateButtons).forEach(key => {
    if (stateButtons[key]) stateButtons[key].classList.remove("active");
  });
  if (stateButtons[stateStr]) {
    stateButtons[stateStr].classList.add("active");
  }
}

function updateControl(baseId, val) {
  const slider = document.getElementById(baseId + "_slider");
  const num = document.getElementById(baseId + "_num");
  if (slider) slider.value = val;
  if (num) num.value = val;
}

function send(msg) {
  if (ws && ws.readyState === WebSocket.OPEN) ws.send(msg);
}

function changeState(stateStr) {
  updateStateButtons(stateStr);
  send("state=" + stateStr);
}

document.getElementById("leg_select").addEventListener("change", (e) => {
  activeLeg = e.target.value;
  send("leg=" + activeLeg);
});

function setLegPowerUI(powerOn) {
  const powerBtn = document.getElementById("leg_power_btn");
  if (powerOn) {
    powerBtn.className = "action-btn on";
    powerBtn.textContent = "POWER OFF";
  } else {
    powerBtn.className = "action-btn off";
    powerBtn.textContent = "POWER ON";
  }
}

function toggleLegPower() {
  const powerBtn = document.getElementById("leg_power_btn");
  const isCurrentlyOn = powerBtn.classList.contains("on");
  const newState = !isCurrentlyOn;

  setLegPowerUI(newState);
  send("leg=" + activeLeg + " enabled=" + (newState ? "1" : "0"));
}

function turnOnAllLegs() {
  setLegPowerUI(true);
  send("leg=all enabled=1");
}

function turnOffAllLegs() {
  setLegPowerUI(false);
  send("leg=all enabled=0");
}

function bindPair(prefix, key, global) {
  const slider = document.getElementById(prefix + "_slider");
  const num = document.getElementById(prefix + "_num");
  const emit = (v) => send(global ? (key + "=" + v) : ("leg=" + activeLeg + " " + key + "=" + v));
  slider.addEventListener("input", () => { num.value = slider.value; emit(slider.value); });
  num.addEventListener("input", () => { slider.value = num.value; emit(num.value); });
}
function bindSingle(id, key) {
  const el = document.getElementById(id);
  el.addEventListener("input", () => send("leg=" + activeLeg + " " + key + "=" + el.value));
}

bindPair("bodyHeight", "bodyHeight", true);
bindPair("bodyWidth", "bodyWidth", true);
bindPair("bodyStride", "bodyStride", true);
bindPair("bodyPitch", "bodyPitch", true);
bindPair("bodyRoll", "bodyRoll", true);

bindPair("W", "W");
bindPair("H", "H");
bindPair("L", "L");
bindPair("offC", "offC");
bindPair("offF", "offF");
bindPair("offT", "offT");
bindPair("servoC", "servoC");
bindPair("servoF", "servoF");
bindPair("servoT", "servoT");

bindSingle("cMin_num", "cMin");
bindSingle("cMax_num", "cMax");
bindSingle("fMin_num", "fMin");
bindSingle("fMax_num", "fMax");
bindSingle("tMin_num", "tMin");
bindSingle("tMax_num", "tMax");
</script>

</body>
</html>
)HTMLDELIM";

#endif