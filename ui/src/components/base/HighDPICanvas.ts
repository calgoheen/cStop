export abstract class HighDPICanvas {
    protected ctx: CanvasRenderingContext2D;
    protected width = 0;
    protected height = 0;

    private resizeHandler: () => void;

    constructor(public canvas: HTMLCanvasElement) {
        this.ctx = this.canvas.getContext("2d")!;

        this.resizeHandler = () => this.resize();
        window.addEventListener("resize", this.resizeHandler);

        requestAnimationFrame(() => this.resize());
    }

    protected resize() {
        const dpr = window.devicePixelRatio || 1;
        const rect = this.canvas.getBoundingClientRect();

        this.width = rect.width;
        this.height = rect.height;

        this.canvas.width = rect.width * dpr;
        this.canvas.height = rect.height * dpr;

        this.ctx.scale(dpr, dpr);

        this.draw();
    }

    protected abstract draw(): void;

    destroy() {
        window.removeEventListener("resize", this.resizeHandler);
    }
}
