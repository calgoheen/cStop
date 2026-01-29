import { HighDPICanvas } from "./base/HighDPICanvas.ts";
import { drawKnob } from "./rendering/draw.ts";
import { snapToLegalValue } from "../utils/math.ts";

export class Knob extends HighDPICanvas {
    private value = 0;
    private numSteps = 0;
    private defaultValue = 0;
    private onChange = (value: number) => {};

    private highlight = false;
    private dragging = false;

    private mouseMove!: (e: MouseEvent) => void;
    private mouseUp!: (e: MouseEvent) => void;

    constructor(canvas: HTMLCanvasElement) {
        super(canvas);

        this.setValue(this.value);
        this.setupMouse();
    }

    draw() {
        this.ctx.clearRect(0, 0, this.width, this.height);
        drawKnob(this.ctx, 0, 0, this.width, this.height, this.value, this.highlight || this.dragging);
    }

    setValue(value: number) {
        this.value = snapToLegalValue(value, this.numSteps);
        this.draw();
    }

    setProperties(numSteps: number, defaultValue: number, onChange: typeof this.onChange) {
        this.numSteps = numSteps;
        this.defaultValue = defaultValue;
        this.onChange = onChange;

        this.setValue(this.value);
    }

    private setupMouse() {
        let startY = 0;
        let startValue = 0;

        this.canvas.addEventListener("mousedown", (e) => {
            this.dragging = true;
            startY = e.clientY;
            startValue = this.value;
        });

        this.canvas.addEventListener("dblclick", () => {
            this.setValue(this.defaultValue);
            this.onChange(this.value);
        });

        this.canvas.addEventListener("mouseenter", () => {
            this.highlight = true;
            this.draw();
        });

        this.canvas.addEventListener("mouseleave", () => {
            this.highlight = false;
            this.draw();
        });

        this.mouseMove = (e) => {
            if (!this.dragging) return;
            const delta = (startY - e.clientY) * (e.shiftKey ? 0.001 : 0.005);

            this.setValue(startValue + delta);
            this.onChange(this.value);
        };

        this.mouseUp = () => {
            this.dragging = false;
            this.draw();
        };

        window.addEventListener("mousemove", this.mouseMove);
        window.addEventListener("mouseup", this.mouseUp);
    }

    destroy() {
        super.destroy();
        window.removeEventListener("mousemove", this.mouseMove);
        window.removeEventListener("mouseup", this.mouseUp);
    }
}
