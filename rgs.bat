rem gswin64c -sDEVICE=tiff24nc -sOutputFile=t.tiff -dNOPAUSE -dBATCH -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -r400 -dDEVICEWIDTHPOINTS=1751 -dDEVICEHEIGHTPOINTS=1751 sky.eps

rem 1200dpi
start gswin64c -sDEVICE=pnggray -sOutputFile=test.png -dNOPAUSE -dBATCH -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -r600 -dDEVICEWIDTHPOINTS=1751 -dDEVICEHEIGHTPOINTS=1751 sky.eps
