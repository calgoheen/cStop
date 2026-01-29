import { HighDPICanvas } from "./base/HighDPICanvas.ts";
import { colors, cornerRadius } from "./rendering/theme.ts";
import { mapFrom0to1, linspace, exponentialCurve } from "../utils/math.ts";
import { Point, Rect } from "../utils/geometry.ts";

export class Curve extends HighDPICanvas {
    private readonly defaultCurveValue = 0.5;
    private readonly defaultStartValue = 0;
    private readonly defaultEndValue = 1;

    private curveValue = 0.5;
    private startValue = 0;
    private endValue = 1;

    private plotRect = Rect.create(0, 0, 0, 0);
    private startRect = Rect.create(0, 0, 0, 0);
    private endRect = Rect.create(0, 0, 0, 0);

    private highlight = -1;
    private dragging = -1;

    private onCurveChange = (val: number) => {};
    private onStartChange = (val: number) => {};
    private onEndChange = (val: number) => {};

    private mouseMove!: (e: MouseEvent) => void;
    private mouseUp!: (e: MouseEvent) => void;

    constructor(
        canvas: HTMLCanvasElement,
        private curveType: "slowdown" | "speedup",
    ) {
        super(canvas);
        this.setupMouse();
    }

    setProperties(onCurveChange: typeof this.onCurveChange, onStartChange: typeof this.onStartChange, onEndChange: typeof this.onEndChange) {
        this.onCurveChange = onCurveChange;
        this.onStartChange = onStartChange;
        this.onEndChange = onEndChange;
    }

    draw() {
        this.ctx.clearRect(0, 0, this.width, this.height);

        this.updateRects();

        this.drawPlot(this.plotRect);
        this.drawCurve(Rect.reduced(this.plotRect, 2, 2), this.curveType, this.curveValue, this.dragging === 0 || this.highlight === 0);

        this.drawHandleBackground(this.plotRect, "start", this.startValue);
        this.drawHandle(this.startRect, this.dragging === 1 || this.highlight === 1);

        this.drawHandleBackground(this.plotRect, "end", this.endValue);
        this.drawHandle(this.endRect, this.dragging === 2 || this.highlight === 2);
    }

    setCurveValue(value: number) {
        this.curveValue = value;
        this.draw();
    }

    setStartValue(value: number) {
        this.startValue = Math.min(value, this.endValue - 0.1);
        this.draw();
    }

    setEndValue(value: number) {
        this.endValue = Math.max(value, this.startValue + 0.1);
        this.draw();
    }

    private updateRects() {
        const padding = 10;
        const handleSize = this.height / 8;

        this.plotRect = Rect.create(padding, padding, this.width - 2 * padding, this.height - 2 * padding);

        this.startRect = Rect.create(
            this.plotRect.x + this.startValue * this.plotRect.width - handleSize / 2,
            this.plotRect.y + this.plotRect.height / 2 - handleSize / 2,
            handleSize,
            handleSize,
        );

        this.endRect = Rect.create(
            this.plotRect.x + this.endValue * this.plotRect.width - handleSize / 2,
            this.plotRect.y + this.plotRect.height / 2 - handleSize / 2,
            handleSize,
            handleSize,
        );
    }

    private drawPlot(rect: Rect) {
        const { x, y, width, height } = rect;

        this.ctx.beginPath();
        this.ctx.roundRect(x, y, width, height, cornerRadius);

        this.ctx.fillStyle = colors.backgroundDarker;
        this.ctx.fill();

        const grid = 4;
        this.ctx.beginPath();

        for (let i = 1; i < grid; i++) {
            const currentX = x + i * (width / grid);
            const currentY = y + i * (height / grid);

            this.ctx.moveTo(currentX, y);
            this.ctx.lineTo(currentX, y + height);
            this.ctx.moveTo(x, currentY);
            this.ctx.lineTo(x + width, currentY);
        }

        this.ctx.lineWidth = 1;
        this.ctx.strokeStyle = colors.backgroundLight;
        this.ctx.stroke();
    }

    private drawCurve(rect: Rect, curveType: "slowdown" | "speedup", curveValue: number, isHighlighted: boolean) {
        const { x, y, width, height } = rect;

        const mapPointToPlot = (normalizedX: number, normalizedY: number) => [mapFrom0to1(normalizedX, x, x + width), mapFrom0to1(normalizedY, y + height, y)];

        const numPoints = 40;
        const xValues = linspace(0, 1, numPoints);
        const yValues = (curveType === "slowdown" ? linspace(1, 0, numPoints) : linspace(0, 1, numPoints)).map((v) =>
            exponentialCurve(v, -(curveValue * 2 - 1)),
        );

        this.ctx.beginPath();

        for (let i = 0; i < numPoints; i++) {
            const [px, py] = mapPointToPlot(xValues[i], yValues[i]);

            if (i == 0) this.ctx.moveTo(px, py);
            else this.ctx.lineTo(px, py);
        }

        this.ctx.lineCap = "round";
        this.ctx.lineWidth = 2;
        this.ctx.strokeStyle = isHighlighted ? colors.highlightLight : colors.highlightDark;
        this.ctx.stroke();
    }

    private drawHandleBackground(plotRect: Rect, handleType: "start" | "end", handleValue: number) {
        this.ctx.beginPath();
        if (handleType === "start") {
            this.ctx.roundRect(plotRect.x, plotRect.y, plotRect.width * handleValue, plotRect.height, [cornerRadius, 0, 0, cornerRadius]);
        } else {
            this.ctx.roundRect(plotRect.x + handleValue * plotRect.width, plotRect.y, plotRect.width * (1 - handleValue), plotRect.height, [
                0,
                cornerRadius,
                cornerRadius,
                0,
            ]);
        }

        this.ctx.fillStyle = colors.highlightDark;
        this.ctx.globalAlpha = 0.5;
        this.ctx.fill();
        this.ctx.globalAlpha = 1;
    }

    private drawHandle(handleRect: Rect, isHighlighted: boolean) {
        const cx = handleRect.x + handleRect.width / 2;
        const cy = handleRect.y + handleRect.height / 2;
        const r1 = handleRect.width / 2;
        const r2 = r1 / 1.5;

        this.ctx.fillStyle = isHighlighted ? colors.highlightLight : colors.highlightDark;
        this.ctx.strokeStyle = this.ctx.fillStyle;
        this.ctx.lineWidth = 1;

        this.ctx.beginPath();
        this.ctx.arc(cx, cy, r2, 0, 2 * Math.PI);
        this.ctx.fill();

        this.ctx.beginPath();
        this.ctx.arc(cx, cy, r1, 0, 2 * Math.PI);
        this.ctx.stroke();
    }

    private getEventComponent(e: MouseEvent) {
        const clientRect = this.canvas.getBoundingClientRect();
        const p = Point.create(e.clientX - clientRect.left, e.clientY - clientRect.top);

        if (Rect.contains(this.startRect, p)) return 1;
        else if (Rect.contains(this.endRect, p)) return 2;
        else if (Rect.contains(this.plotRect, p)) return 0;
        else return -1;
    }

    private setupMouse() {
        let startPos = 0;
        let startValue = 0;

        this.canvas.addEventListener("mousedown", (e) => {
            const eventComponent = this.getEventComponent(e);

            if (eventComponent !== -1) {
                this.dragging = eventComponent;

                if (eventComponent === 0) {
                    startPos = e.clientY;
                    startValue = this.curveValue;
                } else if (eventComponent === 1) {
                    startPos = e.clientX;
                    startValue = this.startValue;
                } else if (eventComponent === 2) {
                    startPos = e.clientX;
                    startValue = this.endValue;
                }
            }
        });

        this.canvas.addEventListener("dblclick", (e) => {
            if (this.getEventComponent(e) === 0) {
                this.setCurveValue(this.defaultCurveValue);
                this.onCurveChange(this.curveValue);
            } else if (this.getEventComponent(e) === 1) {
                this.setStartValue(this.defaultStartValue);
                this.onStartChange(this.startValue);
            } else if (this.getEventComponent(e) === 2) {
                this.setEndValue(this.defaultEndValue);
                this.onEndChange(this.endValue);
            }

            this.highlight = this.getEventComponent(e);
            this.draw();
        });

        this.mouseMove = (e) => {
            if (this.dragging === -1) {
                const eventComponent = this.getEventComponent(e);
                if (this.highlight !== eventComponent) {
                    this.highlight = eventComponent;
                    this.draw();
                }
            }

            if (this.dragging === 0) {
                const delta = (startPos - e.clientY) * (e.shiftKey ? 0.001 : 0.005);
                const newValue = Math.max(0, Math.min(1, startValue + delta));
                this.setCurveValue(newValue);
                this.onCurveChange(this.curveValue);
            } else if (this.dragging === 1) {
                const delta = (e.clientX - startPos) / this.plotRect.width;
                const newValue = Math.max(0, Math.min(1, startValue + delta));
                this.setStartValue(newValue);
                this.onStartChange(this.startValue);
            } else if (this.dragging === 2) {
                const delta = (e.clientX - startPos) / this.plotRect.width;
                const newValue = Math.max(0, Math.min(1, startValue + delta));
                this.setEndValue(newValue);
                this.onEndChange(this.endValue);
            }
        };

        this.mouseUp = (e) => {
            this.dragging = -1;
            this.highlight = this.getEventComponent(e);
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
