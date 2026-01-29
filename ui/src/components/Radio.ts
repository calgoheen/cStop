import { HighDPICanvas } from "./base/HighDPICanvas.ts";
import { drawText, drawToggle } from "./rendering/draw.ts";

export class Radio extends HighDPICanvas {
    private value = 0;
    private choices: string[] = [];
    private valueChanged = (val: number) => {};
    private mouseOverState = -1;
    private isMouseDown = false;

    constructor(canvas: HTMLCanvasElement) {
        super(canvas);

        this.setupMouse();
        this.resize();
    }

    initialize(choices: string[], valueChanged: (val: number) => void) {
        this.choices = choices;
        this.valueChanged = valueChanged;

        this.draw();
    }

    draw() {
        this.ctx.clearRect(0, 0, this.width, this.height);

        const height = this.height / this.choices.length;

        for (let i = 0; i < this.choices.length; i++) {
            drawText(this.ctx, this.choices[i], height + 7, i * height, this.width - height, height, 0.6 * height, false);
            drawToggle(this.ctx, 0, i * height, height, this.mouseOverState === i, this.mouseOverState === i && this.isMouseDown, this.value === i);
        }
    }

    setValue(value: number) {
        this.value = Math.max(0, Math.min(this.choices.length - 1, value));
        this.draw();
    }

    private setupMouse() {
        const getToggleIndex = (clientY: number) => {
            const y = clientY - this.canvas.getBoundingClientRect().top;
            const choiceHeight = this.height / this.choices.length;
            return Math.floor(y / choiceHeight);
        };

        this.canvas.addEventListener("click", (e) => {
            this.setValue(getToggleIndex(e.clientY));
            this.valueChanged(this.value);
        });

        this.canvas.addEventListener("mousemove", (e) => {
            this.mouseOverState = getToggleIndex(e.clientY);
            this.draw();
        });

        this.canvas.addEventListener("mouseleave", () => {
            this.mouseOverState = -1;
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
