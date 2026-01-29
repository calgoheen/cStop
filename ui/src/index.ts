import { initContinuousSlider, initDiscreteSlider, initCurve, initToggle, initRadio, initButton } from "./components/factory/initComponents.ts";
import { colors } from "./components/rendering/theme.ts";
import * as Juce from "./juce/index.js";

interface Params {
    mode: string;
    autoBypass: string;
    sync: string;
    slowdownLength: string;
    slowdownLengthSync: string;
    slowdownCurve: string;
    slowdownStart: string;
    slowdownEnd: string;
    speedupLength: string;
    speedupLengthSync: string;
    speedupCurve: string;
    speedupStart: string;
    speedupEnd: string;
    fadeLength: string;
    crossfadeLength: string;
    filterType: string;
    filterCutoff: string;
    filterResonance: string;
}

// Make theme colors available in CSS
const style = document.createElement("style");
style.textContent = `:root {
  ${Object.entries(colors)
      .map(([key, value]) => `--${key.replace(/([A-Z])/g, "-$1").toLowerCase()}: ${value};`)
      .join("\n")}
}`;
document.head.appendChild(style);

// Disable right click
const isLocalServer = window.__JUCE__.initialisationData.isLocalServer[0] as boolean;
if (!isLocalServer) document.addEventListener("contextmenu", (e) => e.preventDefault());

// Parameter IDs
const params = window.__JUCE__.initialisationData.params[0] as Params;

// Header
const buttons = document.getElementById("buttons")!;

const bypass = initButton(params.mode, 0, "Bypass");
buttons.appendChild(bypass);

const stop = initButton(params.mode, 1, "Stop");
buttons.appendChild(stop);

const start = initButton(params.mode, 2, "Start");
buttons.appendChild(start);

// General controls
const settings = document.getElementById("settings")!;

const autoBypass = initToggle(params.autoBypass);
autoBypass.style.width = "125px";
settings.appendChild(autoBypass);

const sync = initToggle(params.sync);
sync.style.width = "75px";
settings.appendChild(sync);

// Motion controls
const slowdown = document.getElementById("slowdown")!;

const slowdownCurve = initCurve(params.slowdownCurve, params.slowdownStart, params.slowdownEnd, "slowdown");
slowdown.appendChild(slowdownCurve);

const slowdownLength = initContinuousSlider(params.slowdownLength, "Length", "s");
slowdown.appendChild(slowdownLength);

const slowdownLengthSync = initDiscreteSlider(params.slowdownLengthSync, "Length");
slowdown.appendChild(slowdownLengthSync);

const speedup = document.getElementById("speedup")!;

const speedupCurve = initCurve(params.speedupCurve, params.speedupStart, params.speedupEnd, "speedup");
speedup.appendChild(speedupCurve);

const speedupLength = initContinuousSlider(params.speedupLength, "Length", "s");
speedup.appendChild(speedupLength);

const speedupLengthSync = initDiscreteSlider(params.speedupLengthSync, "Length");
speedup.appendChild(speedupLengthSync);

Juce.getSliderState(params.sync).valueChangedEvent.addListener(() => {
    const setDisplay = (elems: HTMLElement[], display: "flex" | "none") => {
        elems.forEach((e) => (e.style.display = display));
    };

    const isSynced = Juce.getSliderState(params.sync).getNormalisedValue() > 0.5;
    setDisplay([slowdownLengthSync, speedupLengthSync], isSynced ? "flex" : "none");
    setDisplay([slowdownLength, speedupLength], isSynced ? "none" : "flex");

    window.dispatchEvent(new Event("resize"));
});

// Filter controls
const filter = document.getElementById("filter")!;

const filterType = initRadio(params.filterType);
filter.appendChild(filterType);

const filterCutoff = initContinuousSlider(params.filterCutoff, "Cutoff");
filter.appendChild(filterCutoff);

const filterResonance = initContinuousSlider(params.filterResonance, "Resonance");
filter.appendChild(filterResonance);
