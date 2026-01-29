import { colors, cornerRadius, strokeWidth } from "./theme.ts";

export function drawToggle(ctx: CanvasRenderingContext2D, x: number, y: number, size: number, isMouseOver: boolean, isMouseDown: boolean, isToggled: boolean) {
    const padding = 2;

    const xInset = x + padding;
    const yInset = y + padding;
    const sizeInset = size - 2 * padding;

    ctx.beginPath();
    ctx.roundRect(xInset, yInset, sizeInset, sizeInset, cornerRadius);
    ctx.fillStyle = colors.backgroundDarker;
    ctx.fill();

    if (isMouseDown) {
        ctx.lineWidth = strokeWidth;
        ctx.strokeStyle = colors.highlightLight;
        ctx.stroke();
    } else if (isMouseOver) {
        ctx.lineWidth = strokeWidth;
        ctx.strokeStyle = colors.highlightDark;
        ctx.stroke();
    }

    if (isToggled) {
        const toggleInset = 4;
        const xToggle = xInset + toggleInset;
        const yToggle = yInset + toggleInset;
        const sizeToggle = sizeInset - 2 * toggleInset;

        ctx.beginPath();
        ctx.roundRect(xToggle, yToggle, sizeToggle, sizeToggle, cornerRadius - 2);
        ctx.fillStyle = colors.highlightDark;
        ctx.fill();
    }
}

export function drawKnob(ctx: CanvasRenderingContext2D, x: number, y: number, width: number, height: number, value: number, isHighlighted: boolean) {
    const cx = x + width / 2;
    const cy = y + height / 2;
    const radius = 0.8 * (Math.min(width, height) / 2);

    const twoPi = 2 * Math.PI;
    const startAngle = (-twoPi * 5) / 8;
    const endAngle = twoPi / 8;
    const angle = value * (endAngle - startAngle) + startAngle;

    ctx.beginPath();
    ctx.arc(cx, cy, radius * 0.8, 0, twoPi);
    ctx.lineWidth = 1;
    ctx.strokeStyle = colors.text;
    ctx.fillStyle = colors.backgroundDarker;
    ctx.fill();

    ctx.lineWidth = 2;

    ctx.beginPath();
    ctx.arc(cx, cy, radius, startAngle, endAngle);
    ctx.strokeStyle = colors.backgroundDarker;
    ctx.stroke();

    ctx.strokeStyle = isHighlighted ? colors.highlightLight : colors.highlightDark;

    ctx.beginPath();
    ctx.arc(cx, cy, radius, startAngle, angle);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(cx + radius * 0.4 * Math.cos(angle), cy + radius * 0.4 * Math.sin(angle));
    ctx.lineTo(cx + radius * 0.75 * Math.cos(angle), cy + radius * 0.75 * Math.sin(angle));
    ctx.stroke();
}

export function drawText(ctx: CanvasRenderingContext2D, text: string, x: number, y: number, width: number, height: number, fontSize = 16, center = true) {
    ctx.font = `${fontSize}px sans-serif`;
    ctx.fillStyle = colors.text;
    ctx.textBaseline = "middle";

    if (center) {
        ctx.textAlign = "center";
        ctx.fillText(text, x + width / 2, y + height / 2, width);
    } else {
        ctx.textAlign = "left";
        ctx.fillText(text, x, y + height / 2, width);
    }
}

export function drawButton(ctx: CanvasRenderingContext2D, x: number, y: number, width: number, height: number, isMouseOver: boolean, isMouseDown: boolean) {
    const padding = 2;

    ctx.beginPath();
    ctx.roundRect(x + padding, y + padding, width - 2 * padding, height - 2 * padding, cornerRadius);
    ctx.fillStyle = isMouseDown ? colors.backgroundLighter : colors.backgroundLight;
    ctx.fill();

    if (isMouseOver || isMouseDown) {
        ctx.lineWidth = strokeWidth;
        ctx.strokeStyle = isMouseDown ? colors.highlightLight : colors.highlightDark;
        ctx.stroke();
    }
}
