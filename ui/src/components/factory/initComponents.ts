import * as Juce from "../../juce/index.js";
import { Knob } from "../Knob.ts";
import { ValueInput } from "../ValueInput.ts";
import { Curve } from "../Curve.ts";
import { Toggle } from "../Toggle.ts";
import { Radio } from "../Radio.ts";
import { Button } from "../Button.ts";
import { formatDisplayValue, convertFrom0to1, convertTo0to1 } from "../../utils/math.ts";

const defaultValues = window.__JUCE__.initialisationData.defaultValues[0];
const choices = window.__JUCE__.initialisationData.choices[0];

function createSlider() {
    const container = document.createElement("div");
    container.className = "column";

    const label = container.appendChild(document.createElement("div"));
    label.className = "text label";

    const knob = new Knob(container.appendChild(document.createElement("canvas")));
    knob.canvas.className = "knob";

    const valueInput = new ValueInput(container.appendChild(document.createElement("input")));
    valueInput.input.className = "value-input";

    return { container, label, knob, valueInput };
}

function attachContinuousSlider(slider: ReturnType<typeof createSlider>, paramId: string, name: string, label = "") {
    slider.label.textContent = name;

    const state = Juce.getSliderState(paramId);

    state.propertiesChangedEvent.addListener(() => {
        slider.knob.setProperties(state.properties.numSteps, defaultValues[paramId], (val) => {
            state.setNormalisedValue(val);
        });

        slider.valueInput.setProperties(
            (val) => {
                const scaledValue = convertFrom0to1(val, state.properties.start, state.properties.end, state.properties.skew);
                return formatDisplayValue(scaledValue, label !== "" ? label : state.properties.label);
            },
            (s) => {
                const val = parseFloat(s);
                if (isNaN(val)) return NaN;

                return convertTo0to1(val, state.properties.start, state.properties.end, state.properties.skew);
            },
            (val) => {
                state.setNormalisedValue(val);
            },
        );
    });

    state.valueChangedEvent.addListener(() => {
        slider.knob.setValue(state.getNormalisedValue());
        slider.valueInput.setValue(state.getNormalisedValue());
    });
}

export function initContinuousSlider(paramId: string, name = "", label = "") {
    const slider = createSlider();
    attachContinuousSlider(slider, paramId, name, label);

    return slider.container;
}

function attachDiscreteSlider(slider: ReturnType<typeof createSlider>, paramId: string, name: string) {
    slider.label.textContent = name;

    const state = Juce.getSliderState(paramId);

    state.propertiesChangedEvent.addListener(() => {
        slider.knob.setProperties(state.properties.numSteps, defaultValues[paramId], (val) => {
            state.setNormalisedValue(val);
        });

        slider.valueInput.setProperties(
            (val) => {
                const scaledValue = convertFrom0to1(val, state.properties.start, state.properties.end, state.properties.skew);
                return choices[paramId][Math.round(scaledValue)];
            },
            (s) => {
                for (let i = 0; i < choices[paramId].length; i++) {
                    if (choices[paramId][i].toLowerCase() === s.toLowerCase()) {
                        return convertTo0to1(i, state.properties.start, state.properties.end, state.properties.skew);
                    }
                }

                return NaN;
            },
            (val) => {
                state.setNormalisedValue(val);
            },
        );
    });

    state.valueChangedEvent.addListener(() => {
        slider.knob.setValue(state.getNormalisedValue());
        slider.valueInput.setValue(state.getNormalisedValue());
    });
}

export function initDiscreteSlider(paramId: string, name: string) {
    const slider = createSlider();
    attachDiscreteSlider(slider, paramId, name);

    return slider.container;
}

export function initCurve(curveParamId: string, startParamId: string, endParamId: string, curveType: "slowdown" | "speedup") {
    const canvas = document.createElement("canvas");
    canvas.className = "curve-plot";

    const curveState = Juce.getSliderState(curveParamId);
    const startState = Juce.getSliderState(startParamId);
    const endState = Juce.getSliderState(endParamId);

    const curve = new Curve(canvas, curveType);

    let numPropertiesChangedEvents = 0;
    const initialize = () => {
        if (++numPropertiesChangedEvents === 3) {
            curve.setProperties(
                (val) => {
                    curveState.setNormalisedValue(val);
                },
                (val) => {
                    startState.setNormalisedValue(val);
                },
                (val) => {
                    endState.setNormalisedValue(val);
                },
            );
        }
    };

    curveState.propertiesChangedEvent.addListener(() => {
        initialize();
    });

    startState.propertiesChangedEvent.addListener(() => {
        initialize();
    });

    endState.propertiesChangedEvent.addListener(() => {
        initialize();
    });

    curveState.valueChangedEvent.addListener(() => {
        curve.setCurveValue(curveState.getNormalisedValue());
    });

    startState.valueChangedEvent.addListener(() => {
        curve.setStartValue(startState.getNormalisedValue());
    });

    endState.valueChangedEvent.addListener(() => {
        curve.setEndValue(endState.getNormalisedValue());
    });

    return canvas;
}

export function initToggle(paramId: string) {
    const canvas = document.createElement("canvas");
    canvas.className = "toggle";

    const state = Juce.getSliderState(paramId);
    const toggle = new Toggle(canvas);

    state.propertiesChangedEvent.addListener(() => {
        toggle.initialize(state.properties.name, (val) => {
            state.setNormalisedValue(val ? 1 : 0);
        });
    });

    state.valueChangedEvent.addListener(() => {
        toggle.setValue(state.getNormalisedValue() > 0.5 ? true : false);
    });

    return canvas;
}

export function initRadio(paramId: string) {
    const canvas = document.createElement("canvas");
    canvas.className = "radio";

    const state = Juce.getSliderState(paramId);
    const radio = new Radio(canvas);

    state.propertiesChangedEvent.addListener(() => {
        radio.initialize(choices[paramId], (val) => {
            state.setNormalisedValue(convertTo0to1(val, 0, choices[paramId].length - 1));
        });
    });

    state.valueChangedEvent.addListener(() => {
        radio.setValue(state.getScaledValue());
    });

    return canvas;
}

export function initButton(paramId: string, value: number, text: string) {
    const canvas = document.createElement("canvas");
    canvas.className = "button";

    const state = Juce.getSliderState(paramId);
    const button = new Button(canvas, text);

    button.onClick = () => {
        state.setNormalisedValue(convertTo0to1(value, 0, choices[paramId].length - 1));
    };

    return canvas;
}
