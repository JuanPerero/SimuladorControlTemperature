var chartObj;  // Declarar fuera de FusionCharts.ready
document.addEventListener('DOMContentLoaded', function() {
    //var chartObj;  // Declarar fuera de FusionCharts.ready
    
    //function updateValue(val) {
    //    if (chartObj) {  // Verificar que chartObj existe antes de usarlo
    //        chartObj.setData(val);
    //        console.log(val);
    //    }
    //} Borrar si funciona bien


    window.tankSim = chartObj
    window.addEventListener("message", function(event) {
        if (event.data.type === "IMGanimation") {
            let currentData = chartObj.getJSONData();
            let items = currentData.annotations.groups.find(g => g.id === "imageGroup").items;
            items.forEach(item => {
                if (item.id === "img1") item.visible = "1";
                if (item.id === "img2") item.visible = "0";
                if (item.id === "img3") item.visible = "1";
                if (item.id === "img4") item.visible = "0";
            });
            chartObj.setJSONData(currentData);
        }
      });
    // Verificar si el contenedor existe
    if (document.getElementById('chart-container')) {
        FusionCharts.ready(function() {
            // Verificar si ya existe una instancia y destruirla
            if (FusionCharts('chartId')) {
                FusionCharts('chartId').dispose();
            }
    //------------------------------------------------------------------
    //--------------    Elemento grafico    ---------------------------- 

            var chartObj = new FusionCharts({
                type: 'angulargauge',
                renderAt: 'chart-container',
                width: '450',
                height: '300',
                dataFormat: 'json',
                dataSource: {
                    "chart": {
                        "caption": "Temperatura del líquido en el contenedor",
                        "subcaption": "Experimento del calentador de agua",
                        "plotToolText": "Current Score: $value",
                        "theme": "fusion",
                        "chartBottomMargin": "50",
                        "refreshInterval": "0.1",
                        "showValue": "1"
                    },
                    "colorRange": {
                        "color": [{
                            "minValue": "0",
                            "maxValue": "4.5",
                            "code": "#080087"
                        },{
                            "minValue": "4.5",
                            "maxValue": "16",
                            "code": "#1a94a7"
                        },{
                            "minValue": "16",
                            "maxValue": "22",
                            "code": "#1aa771"
                        }, {
                            "minValue": "22",
                            "maxValue": "28",
                            "code": "#32bb24"
                        }, {
                            "minValue": "26",
                            "maxValue": "40",
                            "code": "#7dc00d"
                        }, {
                            "minValue": "40",
                            "maxValue": "60",
                            "code": "#ff8b0c"
                        }, {
                            "minValue": "60",
                            "maxValue": "80",
                            "code": "#901a1a"
                        }, {
                            "minValue": "80",
                            "maxValue": "100",
                            "code": "#d10000"
                        }]
                    },
                    "dials": {
                        "dial": [{
                            "value": "8.9"
                        }]
                    },
                    "trendPoints": {
                        "point": [{
                            "startValue": "6.8",
                            "color": "#0075c2",
                            "dashed": "1"
                        }, {
                            "startValue": "9.5",
                            "color": "#0075c2",
                            "dashed": "1"
                        }, {
                            "startValue": "6.8",
                            "endValue": "9.5",
                            "color": "#0075c2",
                            "radius": "185",
                            "innerRadius": "80"
                        }]
                    },
                    "annotations": {
                        "origw": "450",
                        "origh": "300",
                        "autoscale": "1",
                        "showBelow": "0",
                        "groups": [{
                            "id": "arcs",
                            "items": [
                            {
                                "id": "store-cs-bg",
                                "type": "rectangle",
                                "x": "$chartCenterX-130",
                                "y": "$chartEndY - 20",
                                "tox": "$chartCenterX + 150",
                                "toy": "$chartEndY - 2",
                                "fillcolor": "#0075c2"
                            }, {
                                "id": "state-cs-text",
                                "type": "Text",
                                "color": "#ffffff",
                                "label": "Referencia \u00B1 1%",
                                "fontSize": "12",
                                "align": "center",
                                "x": "$chartCenterX + 10",
                                "y": "$chartEndY - 12"
                            }]
                        }]
                    }
                }
            }
            );

            //------------------------------------------------------------------    
            //------------------------------------------------------------------                
            // Solo intentar renderizar si el contenedor existe
            if (document.getElementById('chart-container')) {
                chartObj.render();
            } else {
                console.error("El contenedor 'chart-container' no existe en el DOM");
            }
            window.tankSim = chartObj
        });
    } else {
        console.error("El contenedor 'chart-container' no existe en el DOM al cargar la página");
    }
});