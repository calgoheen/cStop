export type Point = { x: number; y: number };
export type Rect = { x: number; y: number; width: number; height: number };

export namespace Point {
    export function create(x: number, y: number) {
        return { x, y };
    }
}

export namespace Rect {
    export function create(x: number, y: number, width: number, height: number) {
        return { x, y, width, height };
    }

    export function contains(rect: Rect, point: Point): boolean {
        return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y && point.y <= rect.y + rect.height;
    }

    export function reduced(rect: Rect, paddingX: number, paddingY: number) {
        return {
            x: rect.x + paddingX,
            y: rect.y + paddingY,
            width: rect.width - paddingX * 2,
            height: rect.height - paddingY * 2,
        };
    }
}
