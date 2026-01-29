import { HighDPICanvas } from "./base/HighDPICanvas.ts";
import { drawText, drawToggle } from "./rendering/draw.ts";

export class Toggle extends HighDPICanvas {
    private value = false;
    private name = "";
    private valueChanged = (val: boolean) => {};
    private isMouseOver = false;
    private isMouseDown = false;

    constructor(canvas: HTMLCanvasElement) {
        super(canvas);

        this.setupMouse();
        this.resize();
    }

    initialize(name: string, valueChanged: (val: boolean) => void) {
        this.name = name;
        this.valueChanged = valueChanged;

        this.draw();
    }

    draw() {
        this.ctx.clearRect(0, 0, this.width, this.height);

        const textX = this.height + 6;

        drawText(this.ctx, this.name, textX, 0, this.width - textX, this.height, 0.5 * this.height, false);
        drawToggle(this.ctx, 0, 0, this.height, this.isMouseOver, this.isMouseDown, this.value);
    }

    setValue(value: boolean) {
        this.value = value;
        this.draw();
    }

    private setupMouse() {
        this.canvas.addEventListener("click", () => {
            this.setValue(!this.value);
            this.valueChanged(this.value);
        });

        this.canvas.addEventListener("mouseenter", () => {
            this.isMouseOver = true;
            this.draw();
        });

        this.canvas.addEventListener("mouseleave", () => {
            this.isMouseOver = false;
            this.draw();
        });

        this.canvas.addEventListener("mousedown", () => {
            this.isMouseDown = true;
            this.draw();
        });

        this.canvas.addEventListener("mouseup", () => {
            this.isMouseDown = false;
            this.draw();
        });
    }
}
