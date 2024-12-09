<?php
$stocks = explode(",", $_GET['stocks']);
$displays = ['interval' => '20000', 'displays' => []];
foreach ($stocks as $stock) {
	$stockData = outputStockDisplay($stock);
	$displays['displays'][] = $stockData;
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($displays);
exit;

function get_stock_data($symbol)
{
	$apiKey = 'aVhwT1NWWkhIZVBRZlIwOUlHb01keWFrMEI5Ql9QM1ZIZndtay1ub0V3OD0';
	$url = sprintf('https://api.marketdata.app/v1/stocks/quotes/%s/?token=%s', $symbol, $apiKey);
	$json = file_get_contents($url);
	return json_decode($json, true);
}

function outputStockDisplay($symbol)
{
	$data = get_stock_data($symbol);
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
            [
                'type' => 'rectangle',
                'x' => 100,
                'y' => 185,
                'width' => 40,
                'height' => 40,
                'filled' => true,
                'color' => 'black',
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
