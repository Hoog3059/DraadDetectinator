<template>
    <Scatter :chart-options="chartOptions" :chart-data="chartData" :chart-id="chartId" :dataset-id-key="datasetIdKey"
        :plugins="plugins" :css-classes="cssClasses" :styles="styles" />
</template>

<script>
import { Scatter } from 'vue-chartjs';
import {
    Chart,
    ArcElement,
    LineElement,
    BarElement,
    PointElement,
    BarController,
    BubbleController,
    DoughnutController,
    LineController,
    PieController,
    PolarAreaController,
    RadarController,
    ScatterController,
    CategoryScale,
    LinearScale,
    LogarithmicScale,
    RadialLinearScale,
    TimeScale,
    TimeSeriesScale,
    Decimation,
    Filler,
    Legend,
    Title,
    Tooltip,
    SubTitle
} from 'chart.js';

Chart.register(
    ArcElement,
    LineElement,
    BarElement,
    PointElement,
    BarController,
    BubbleController,
    DoughnutController,
    LineController,
    PieController,
    PolarAreaController,
    RadarController,
    ScatterController,
    CategoryScale,
    LinearScale,
    LogarithmicScale,
    RadialLinearScale,
    TimeScale,
    TimeSeriesScale,
    Decimation,
    Filler,
    Legend,
    Title,
    Tooltip,
    SubTitle
);

export default {
    name: 'OscilloscopeComponent',
    components: { Scatter },
    props: {
        chartPointData: {
            type: Object,
            default: () => { }
        },
        chartId: {
            type: String,
            default: 'osci-chart'
        },
        datasetIdKey: {
            type: String,
            default: 'label'
        },
        width: {
            type: Number,
            default: 400
        },
        height: {
            type: Number,
            default: 400
        },
        cssClasses: {
            default: '',
            type: String
        },
        styles: {
            type: Object,
            default: () => { }
        },
        plugins: {
            type: Object,
            default: () => { }
        }
    },
    computed: {
        chartData() {
            let coords = this.chartPointData[0].map((v, i) => ({ x: v, y: this.chartPointData[1][i] }));
            return {
                datasets: [
                    {
                        label: 'Voltage vs Time',
                        data: coords,
                        backgroundColor: '#0099ff',
                        borderColor: '#0099ff'
                    }
                ]
            };
        }
    },
    data() {
        return {
            chartOptions: {
                responsive: true,
                maintainAspectRatio: false,
                showLine: true,
                animation: false,
                scales: {
                    y: {
                        title: {
                            display: true,
                            text: 'Measured voltage (V)'
                        },
                        min: 0,
                        max: 3.5
                    },
                    x: {
                        title: {
                            display: true,
                            text: 'Time (ms)'
                        }
                    }
                }
            }
        }
    }
}
</script>