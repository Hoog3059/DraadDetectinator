<template>
    <HeaderBar v-bind:disconnected="disconnected" />

    <span class="tag">
        Last updated: {{ getDatetimeFromTimestamp() }}
    </span>

    <div class="columns is-desktop content">
        <div class="column is-half">
            <div class="grid left">
                <div class="item1 left">
                    <h3 class="title is-3">Oscilloscope</h3>
                    <OscilloscopeComponent :chart-point-data="osciPointData" :css-classes="`chart`" />
                </div>
                <div class="item2 left">
                    <div class="columns is-mobile">
                        <div class="column is-half">
                            <h5 class="title is-5 tight">General</h5>
                            <ul class="datalist">
                                <li>Vmax = {{ parseFloat(Vmax).toFixed(2) }} V</li>
                                <li>Vmin = {{ parseFloat(Vmin).toFixed(2) }} V</li>
                                <li>Vavg = {{ parseFloat(osciPointData[1].reduce((partialSum, a) => partialSum + a,
                                        0) /
                                        osciPointData[1].length).toFixed(2)
                                }} V</li>
                            </ul>
                        </div>
                        <div class="column is-half">
                            <h5 class="title is-5 tight">Peaks</h5>
                            <ul class="datalist">
                                <li>Vptp = {{ parseFloat(Vptp).toFixed(2) }} V</li>
                                <li>Vmax - Vmin = {{ parseFloat(Vmax - Vmin).toFixed(2) }} V</li>
                                <li>Peak width = {{ parseFloat(peakWidth).toFixed(2) }} ms</li>
                            </ul>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <div class="column is-half">
            <div class="grid right">
                <div class="item1">
                    <h3 class="title is-3">Device info</h3>
                    <h5 class="title is-5">Activated sensors</h5>
                    <div class="columns is-mobile">
                        <div class="column is-one-quarter">
                            <div class="columns is-gapless is-multiline">
                                <div class="column is-full sensor" v-for="n in 5" v-bind:key="n"
                                    v-bind:class="isSwitchActivated(activatedSwitches, n) ? 'activated' : ''">
                                </div>
                            </div>
                        </div>
                        <div class="column">
                            <img src="./assets/DraadDetectinator.png" class="detectorImage" />
                        </div>
                    </div>
                </div>
                <div class="item2">
                    <h5 class="title is-5 margin-top">LCD</h5>
                    Current mode: {{ currentModeName }}
                    <div>
                        <div class="LCD">
                            {{ lcdFirstLine }}<br>
                            {{ lcdSecondLine }}
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { io } from "socket.io-client";
import HeaderBar from "./components/HeaderBar.vue";
import OscilloscopeComponent from "./components/OscilloscopeComponent.vue";

export default {
    name: 'App',
    components: {
        HeaderBar,
        OscilloscopeComponent
    },
    socket: {
    },
    data() {
        this.oscichart = {};
        return {
            disconnected: true,
            timestamp: 1666337177000,
            currentMode: 0,
            activatedSwitches: 0,
            Vmax: 0,
            Vmin: 0,
            Vptp: 0,
            peakWidth: 0,
            loopTime: 0,
            osciPointData: [[0, 1000], [0, 0]],
            lcdFirstLine: "",
            lcdSecondLine: ""
        }
    },
    created() {
        this.socket = io(process.env.VUE_APP_SOCKETIO_SERVER);

    },
    mounted() {
        this.socket.on("connect", () => {
            this.disconnected = false;
            console.log("[Socket] Connected to server.");
        });

        this.socket.on("disconnect", () => {
            this.disconnected = true;
            console.error("[Socket] Disconnected from server.");
        });

        this.socket.on("data_update", (arg) => {
            console.log(arg);

            this.timestamp = arg['timestamp'];
            this.loopTime = arg['loopTime'];

            var y = arg['voltageArray'];
            var x = [];
            for (let index = 0; index < y.length; index++) {
                x[index] = index * arg['loopTime'];
            }
            this.osciPointData = [x, y];

            this.Vptp = arg['Vptp'];
            this.Vmax = arg['Vmax'];
            this.Vmin = Math.min(...this.osciPointData[1]);
            this.peakWidth = arg['peakWidth'];

            this.currentMode = arg['currentMode'];
            this.activatedSwitches = arg['activatedSwitches'];

            this.lcdFirstLine = arg['lcdFirstLine'].replace('\\approx', '\u2248');
            this.lcdSecondLine = arg['lcdSecondLine'];
        });
    },
    methods: {
        getDatetimeFromTimestamp: function () {
            const date = new Date(this.timestamp);

            let options = {
                timeZone: 'Europe/Amsterdam',
                year: 'numeric',
                month: 'numeric',
                day: 'numeric',
                hour: 'numeric',
                minute: 'numeric',
                second: 'numeric',
                fractionalSecondDigits: 3
            }

            return date.toLocaleString('nl-NL', options);
        },
        isSwitchActivated: function (activatedSwitches, position) {
            return Boolean((activatedSwitches >> (position - 1)) & 0b00001);
        },
        getModeName: function (mode) {
            switch (mode) {
                case 0:
                    return "Position";
                case 1:
                    return "Depth";
            }
        }
    },
    computed: {
        currentModeName() {
            switch (this.currentMode) {
                case 0:
                    return "Position";
                case 1:
                    return "Depth";
                default:
                    return "Undefined";
            }
        }
    }
};
</script>

<style>
@import "https://cdn.jsdelivr.net/npm/bulma@0.9.4/css/bulma.min.css";
@import "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.2.0/css/fontawesome.min.css";

html,
body {
    height: 100%;
    margin: 0;
}

#app {
    display: flex;
    flex-flow: column;
    height: 100%;
}

.LCD {
    margin-left: auto;
    margin-right: auto;
    padding: 1ch 2ch 1ch 2ch;
    width: 20ch;
    background-color: blue;
    color: white;
    font-family: 'Lucida Sans Typewriter';
    font-variant-numeric: tabular-nums;
    font-size: 24px;
}

.datalist {
    list-style: none;
}

.title.tight {
    margin-bottom: 0;
}

.title.margin-top {
    margin-top: 1em;
}

.detectorImage {
    transform: scaleX(-1);
    width: auto;
    height: auto;
    max-height: 260px;
    margin-left: auto;
    margin-right: auto;
    display: block;
}

.sensor {
    background-color: red;
    margin-top: 10px !important;
    margin-bottom: 10px !important;
    height: 2em;
}

.sensor.activated {
    background-color: green !important;
}

.grid.left {
    padding: 10px;
    display: grid;
    height: 100%;
    grid-template-rows: auto min-content;
    grid-template-columns: 100%;
}

.grid.right {
    padding: 10px;
    display: grid;
    height: 100%;
    grid-template-rows: min-content min-content;
    grid-template-columns: 100%;
}

.item1 {
    grid-row-start: 1;
    grid-row-end: 2;
}

.item2 {
    grid-row-start: 2;
    grid-row-end: 3;
}

.content {
    height: 100%;
}

.chart {
    height: 85%;
}
</style>
