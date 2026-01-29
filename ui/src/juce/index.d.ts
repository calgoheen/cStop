interface JUCEBackend {
    addEventListener(eventName: string, callback: (event: any) => void): void;
    emitEvent(eventName: string, data?: any): void;
}

interface JUCEInitialisationData {
    params: any[];
    defaultValues: any[];
    choices: any[];
    isLocalServer: any[];
}

interface JUCE {
    backend: JUCEBackend;
    initialisationData: JUCEInitialisationData;
}

declare global {
    interface Window {
        __JUCE__: JUCE;
    }
}

export interface SliderProperties {
    start: number;
    end: number;
    skew: number;
    name: string;
    label: string;
    numSteps: number;
    interval: number;
    parameterIndex: number;
}

export class SliderState {
    constructor(name: string);

    readonly name: string;
    readonly properties: SliderProperties;
    readonly valueChangedEvent: {
        addListener(fn: () => void): number;
        removeListener(id: number): void;
    };
    readonly propertiesChangedEvent: {
        addListener(fn: () => void): number;
        removeListener(id: number): void;
    };

    setNormalisedValue(newValue: number): void;
    sliderDragStarted(): void;
    sliderDragEnded(): void;
    getScaledValue(): number;
    getNormalisedValue(): number;
}

export function getNativeFunction(name: string): (...args: any[]) => Promise<any>;
export function getSliderState(name: string): SliderState;
