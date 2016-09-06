$(function() {

    Morris.Area({
        element: 'morris-area-chart',
        data: [{
            time: '133401',
            enp2s0: 3401,
            enp6s0: 3400,
            enp7s0: 2647
        }, {
            time: '133402',
            enp2s0: 2778,
            enp6s0: 2294,
            enp7s0: 2441
        }, {
            time: '133403',
            enp2s0: 4912,
            enp6s0: 1969,
            enp7s0: 2501
        }, {
            time: '133404',
            enp2s0: 3767,
            enp6s0: 3597,
            enp7s0: 5689
        }, {
            time: '133411',
            enp2s0: 6810,
            enp6s0: 1914,
            enp7s0: 2293
        }, {
            time: '133412',
            enp2s0: 5670,
            enp6s0: 4293,
            enp7s0: 1881
        }, {
            time: '133413',
            enp2s0: 4820,
            enp6s0: 3795,
            enp7s0: 1588
        }, {
            time: '133414',
            enp2s0: 15073,
            enp6s0: 5967,
            enp7s0: 5175
        }, {
            time: '133421',
            enp2s0: 10687,
            enp6s0: 4460,
            enp7s0: 2028
        }, {
            time: '133422',
            enp2s0: 8432,
            enp6s0: 5713,
            enp7s0: 1791
        }],
        xkey: 'time',
        ykeys: ['enp2s0', 'enp6s0', 'enp7s0'],
        labels: ['enp2s0', 'enp6s0', 'enp7s0'],
        pointSize: 2,
        hideHover: 'auto',
        resize: true
    });

    Morris.Donut({
        element: 'morris-donut-chart',
        data: [{
            label: "enp2s0",
            value: 5
        }, {
            label: "enp5s0",
            value: 5
        }, {
            label: "enp6s0",
            value: 35
        }, {
            label: "enp7s0",
            value: 5
        }, {
            label: "enp8s0",
            value: 5
        }, {
            label: "enp9s0",
            value: 5
        }],
        resize: true
    });

    Morris.Bar({
        element: 'morris-bar-chart',
        data: [{
            time: '20:06',
            enp2s0: 100,
            enp7s0: 10,
            enp8s0: 10,
	    enp6s0: 90
	    
        }, {
            time: '20:07',
            enp2s0: 75,
            enp7s0: 10,
            enp8s0: 10,
            enp6s0: 65
        }, {
            time: '20:08',
            enp2s0: 50,
            enp7s0: 10,
            enp8s0: 10,
            enp6s0: 40
        }, {
            time: '20:09',
            enp2s0: 75,
            enp7s0: 10,
            enp8s0: 10,
            enp6s0: 65
        }, {
            time: '20:10',
            enp2s0: 50,
            enp7s0: 10,
            enp8s0: 10,
            enp6s0: 40
        }, {
            time: '20:11',
            enp2s0: 75,
            enp7s0: 10,
            enp8s0: 10,
            enp6s0: 65
        }, {
            time: '20:12',
            enp2s0: 100,
            enp7s0: 10,
            enp8s0: 10,
            enp6s0: 90
        }],
        xkey: 'time',
		ykeys: ['enp2s0', 'enp6s0','enp7s0','enp8s0'],
		labels: ['enp2s0', 'enp6s0', 'enp7s0','enp8s0'],
        hideHover: 'auto',
        resize: true
    });
    
});
