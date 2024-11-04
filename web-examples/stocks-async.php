<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
$stocks = explode(",", $_GET['stocks']);
$displays = ['interval' => '20000', 'displays' => []];

$stockData = requestStockData($stocks);
foreach ($stockData as $stock => $data) {
    $displays['displays'][] = outputStockDisplay($stock, $data);
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($displays);
exit;

function requestStockData($stocks)
{
    $running = null;
    $mh = curl_multi_init();

    $apiKey = 'aVhwT1NWWkhIZVBRZlIwOUlHb01keWFrMEI5Ql9QM1ZIZndtay1ub0V3OD0';
    $ch = [];
    foreach ($stocks as $idx => $stock) {
        $ch[$idx] = curl_init();
        $url = sprintf('https://api.marketdata.app/v1/stocks/quotes/%s/?token=%s', $stock, $apiKey);
        curl_setopt($ch[$idx], CURLOPT_URL, $url);
        curl_setopt($ch[$idx], CURLOPT_RETURNTRANSFER, 1);


        // Other curl options....
        curl_multi_add_handle($mh, $ch[$idx]);
    }

    do {
        curl_multi_exec($mh, $running);
        curl_multi_select($mh);
    } while ($running > 0);

    $stockData = [];
    foreach ($stocks as $idx => $stock) {
        $stockData[$stock] = json_decode(curl_multi_getcontent($ch[$idx]), true);
        curl_multi_remove_handle($mh, $ch[$idx]);
    }
    curl_multi_close($mh);

    return $stockData;
}


function outputStockDisplay($symbol, $data)
{
    $price = $data['last'][0];
    $change = $data['change'][0];
    $changePercent = $data['changepct'][0];
    $changePercent = round($changePercent * 100, 2);

    $headerColor = 'blue';

    $output = [
        'fullDraw' => false,
        'data' => [
            [
                'type' => 'rectangle',
                'x' => 0,
                'y' => 0,
                'height' => 50,
                'width' => 240,
                'color' => $headerColor,
                'filled' => true,
            ],
            [
                'type' => 'text',
                'x' => 120,
                'y' => 27,
                'font' => 1,
                'size' => 4,
                'text' => $symbol,
                'align' => 'center',
                'color' => 'white',
                'background' => $headerColor,
            ],
            [
                'type' => 'rectangle',
                'x' => 0,
                'y' => 50,
                'width' => 240,
                'height' => 190,
                'filled' => true,
                'color' => 'black',
            ],
            [
                'type' => 'text',
                'x' => 120,
                'y' => 51 + 32,
                'font' => 1,
                'size' => 4,
                'align' => 'center',
                'text' => sprintf('$%.2f', $price),
                'color' => 'white',
                'background' => 'black',
            ],
            [
                'type' => 'text',
                'x' => 120,
                'y' => 147,
                'font' => 1,
                'size' => 4,
                'align' => 'center',
                'text' => sprintf('%.2f%%', $changePercent),
                'color' => 'green',
                'background' => 'black',
            ],
        ]
    ];

    if ($change > 0) {
        $output['data'][] = [
            'type' => 'triangle',
            'x' => 120,
            'y' => 185,
            'x2' => 140,
            'y2' => 220,
            'x3' => 100,
            'y3' => 220,
            'filled' => true,
            'color' => 'green',
        ];
    } else {
        $output['data'][] = [
            'type' => 'triangle',
            'x' => 120,
            'y' => 220,
            'x2' => 140,
            'y2' => 185,
            'x3' => 100,
            'y3' => 185,
            'filled' => true,
            'color' => 'red',
        ];
    }
    return $output;
}
