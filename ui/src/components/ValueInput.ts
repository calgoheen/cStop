export class ValueInput {
    private value = 0;
    private stringFromValue = (value: number) => value.toString();
    private valueFromString = (s: string) => parseFloat(s) || 0;
    private onChange = (value: number) => {};

    private isActive = false;
    private mouseDown!: (e: MouseEvent) => void;

    constructor(public input: HTMLInputElement) {
        this.input.type = "text";
        this.input.spellcheck = false;
        this.input.autocomplete = "off";
        this.input.autocapitalize = "off";
        this.input.readOnly = true;

        this.setValue(this.value);
        this.setupListeners();
    }

    setValue(value: number) {
        this.value = value;
        this.input.value = this.stringFromValue(value);
    }

    setProperties(stringFromValue: typeof this.stringFromValue, valueFromString: typeof this.valueFromString, onChange: typeof this.onChange) {
        this.stringFromValue = stringFromValue;
        this.valueFromString = valueFromString;
        this.onChange = onChange;

        this.setValue(this.value);
    }

    private enable() {
        this.isActive = true;
        this.input.readOnly = false;

        this.input.focus();
        this.input.select();

        setTimeout(() => {
            window.addEventListener("mousedown", this.mouseDown);
        }, 1);
    }

    private disable() {
        this.isActive = false;
        this.input.readOnly = true;
        this.input.blur();

        window.removeEventListener("mousedown", this.mouseDown);
    }

    private commitTextValue() {
        const val = this.valueFromString(this.input.value);

        if (!isNaN(val)) {
            this.setValue(val);
            this.onChange(val);
        } else {
            this.setValue(this.value);
        }

        this.disable();
    }

    private setupListeners() {
        this.mouseDown = (e) => {
            if (this.isActive && e.target !== this.input) {
                this.commitTextValue();
            }
        };

        this.input.addEventListener("keydown", (e) => {
            if (e.key === "Enter" || e.key === "Escape") {
                e.preventDefault();
                e.stopPropagation();

                if (e.key === "Enter") {
                    this.commitTextValue();
                } else {
                    this.disable();
                    this.setValue(this.value);
                }
            }
        });

        this.input.addEventListener("click", () => {
            this.enable();
        });
    }
}
