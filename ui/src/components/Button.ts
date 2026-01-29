import { HighDPICanvas } from "./base/HighDPICanvas.ts";
import { drawText, drawButton } from "./rendering/draw.ts";

export class Button extends HighDPICanvas {
    public onClick = () => {};
    private isMouseOver = false;
    private isMouseDown = false;

    constructor(
        canvas: HTMLCanvasElement,
        private text: string,
    ) {
        super(canvas);

        this.canvas.addEventListener("click", (e) => {
            this.onClick();
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

    draw() {
        this.ctx.clearRect(0, 0, this.width, this.height);

        drawButton(this.ctx, 0, 0, this.width, this.height, this.isMouseOver, this.isMouseDown);
        drawText(this.ctx, this.text, 0, 0, this.width, this.height, 0.35 * this.height);
    }
}
