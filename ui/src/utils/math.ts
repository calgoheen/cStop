export function clamp(value: number, min: number, max: number) {
    return Math.max(min, Math.min(max, value));
}

export function clampTo0to1(value: number) {
    return clamp(value, 0, 1);
}

export function snapToLegalValue(value: number, numSteps = 0) {
    let newValue = clampTo0to1(value);
    if (numSteps > 0) newValue = Math.round(newValue * (numSteps - 1)) / (numSteps - 1);
    return newValue;
}

export function mapTo0to1(value: number, start: number, end: number) {
    return (value - start) / (end - start);
}

export function mapFrom0to1(normalizedValue: number, start: number, end: number) {
    return start + normalizedValue * (end - start);
}

export function map(srcValue: number, srcRangeStart: number, srcRangeEnd: number, destRangeStart: number, destRangeEnd: number) {
    return mapFrom0to1(mapTo0to1(srcValue, srcRangeStart, srcRangeEnd), destRangeStart, destRangeEnd);
}

export function linspace(start: number, end: number, count: number): number[] {
    const step = (end - start) / (count - 1);
    return Array.from({ length: count }, (_, i) => start + step * i);
}

export function exponentialCurve(x: number, curve: number) {
    console.assert(x >= 0 && x <= 1, "x must be between 0 and 1");
    console.assert(curve >= -1 && curve <= 1, "curve must be between -1 and 1");

    if (Math.abs(curve) < 1e-3) return x;

    const base = 1e6;

    if (curve > 0) return (Math.pow(base, curve * x) - 1) / (Math.pow(base, curve) - 1);
    else return 1 - (Math.pow(base, -curve * (1 - x)) - 1) / (Math.pow(base, -curve) - 1);
}

export function formatDisplayValue(val: number, label: string) {
    let rounded;
    if (val < 10) rounded = Math.round(val * 100) / 100;
    else if (val < 100) rounded = Math.round(val * 10) / 10;
    else if (val < 1000) rounded = Math.round(val);
    else if (val < 10000) rounded = Math.round(val / 10) * 10;
    else rounded = Math.round(val / 100) * 100;

    if (rounded >= 1000) {
        const formatted = (rounded / 1000).toString();
        return label ? `${formatted} k${label}` : `${formatted}k`;
    } else {
        return label ? `${rounded} ${label}` : `${rounded}`;
    }
}

export function convertTo0to1(v: number, start: number, end: number, skew: number = 1) {
    let proportion = clampTo0to1((v - start) / (end - start));

    if (skew === 1) {
        return proportion;
    }

    return Math.pow(proportion, skew);
}

export function convertFrom0to1(proportion: number, start: number, end: number, skew: number = 1) {
    proportion = clampTo0to1(proportion);

    if (skew !== 1 && proportion > 0) {
        proportion = Math.exp(Math.log(proportion) / skew);
    }

    return start + (end - start) * proportion;
}
