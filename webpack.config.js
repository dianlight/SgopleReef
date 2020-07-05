const path = require('path');
const CompressionPlugin = require('compression-webpack-plugin');
const TerserPlugin = require('terser-webpack-plugin');
const HtmlReplaceWebpackPlugin = require('html-replace-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;

let read = 0;
let genv, gargv;


var config = {
    entry: {
        main: path.resolve(__dirname, './src/node/index.js'),
        //    remoteDebugApp: path.resolve(__dirname, './src/node/RemoteDebugApp/index.html')
    },
    output: {
        //    filename: '[name].[id].[ext]',
        path: path.resolve(__dirname, 'data'),
        //    publicPath: "/"
    },
    plugins: [
        new MiniCssExtractPlugin({
            // Options similar to the same options in webpackOptions.output
            // both options are optional
            filename: '[name].css',
            chunkFilename: '[id].css',
        }),
        new HtmlWebpackPlugin({
            template: '!!html-loader!src/node/index.html',
        }),
        new HtmlReplaceWebpackPlugin([{
                pattern: '@@title',
                replacement: 'html replace webpack plugin'
            },
            {
                pattern: /(\{\{ ([\w-_\.\(\)\/]+) \s*\}\})/g,
                replacement: function(match, command) {
                    // those formal parameters could be:
                    // match: <-- env:SMT_VERSION-->
                    // type: argv, argvenv, webpackenv, npmenv
                    // value: SMT_VERSION
                    //          console.log("--------",genv,"----------",process.env,"----------",gargv,'-------------');
                    //          console.log(match,command);
                    let argv = gargv;
                    const titleize = require('titleize');
                    console.log(command, JSON.stringify(eval(command)));
                    return eval(command);
                }
            }
        ]),
        //        new BundleAnalyzerPlugin({
        //            analyzerMode: "disabled",
        //           generateStatsFile: false
        //        }),
    ],
    module: {
        rules: [{
                test: /\.font\.js/,
                use: [
                    MiniCssExtractPlugin.loader,
                    'css-loader',
                    'webfonts-loader'
                ]
            },
            {
                test: /\.css(\?mtime=\d+)?$/i,
                use: [process.env.NODE_ENV !== 'production' ?
                    'style-loader' :
                    MiniCssExtractPlugin.loader, {
                        loader: 'css-loader',
                        options: {
                            sourceMap: process.env.NODE_ENV !== 'production',
                        },
                    },
                ]
            },
            {
                test: /\.jpe?g$|\.ico$|\.gif$|\.png$|\.svg$|\.woff$|\.ttf$|\.wav$|\.mp3$/,
                loader: 'file-loader?name=[name].[ext]' // <- retain original file name
            },
            {
                test: /\.html$/,
                use: ['file-loader?name=[name].[ext]',
                    {
                        loader: 'extract-loader',
                        options: {
                            name: '[path][name].[ext]',
                            //                  publicPath: (url,resourcePath,context) => { 
                            //                    console.log("----------------------------------------------------------");
                            //                    console.log(url,resourcePath,context);
                            //                    return '../'.repeat(path.relative(path.resolve('src/node'), context.context).split('/').length); 
                            //                  },
                        }
                    },
                    {
                        loader: 'html-loader',
                        options: {
                            minimize: process.env.NODE_ENV === 'production',
                            interpolate: true,
                            esModule: false,
                            attrs: ['img:src', 'link:href', 'iframe:src'],
                        }
                    },
                ]
            },
            {
                test: /\.js$/,
                use: [
                    "source-map-loader",
                ],
                enforce: "pre"
            },
            {
                test: /\.less$/,
                use: [
                    process.env.NODE_ENV !== 'production' ?
                    'style-loader' :
                    MiniCssExtractPlugin.loader, {
                        loader: 'css-loader',
                        options: {
                            sourceMap: process.env.NODE_ENV !== 'production',
                        },
                    },
                    { loader: 'less-loader' }, // compiles Less to CSS
                ]
            },
            {
                test: /\.s[ac]ss$/i,
                use: [
                    process.env.NODE_ENV !== 'production' ?
                    'style-loader' :
                    MiniCssExtractPlugin.loader,
                    {
                        loader: 'css-loader',
                        options: {
                            sourceMap: process.env.NODE_ENV !== 'production',
                        },
                    },
                    {
                        loader: 'sass-loader',
                        options: {
                            sourceMap: process.env.NODE_ENV !== 'production',
                            sassOptions: {
                                outputStyle: 'compressed',
                            }
                        },
                    },
                ],
            },
        ]
    },
    optimization: {
        minimize: process.env.NODE_ENV === 'production',
        minimizer: [new TerserPlugin()],
    },
    devServer: {
        contentBase: path.join(__dirname, 'data'),
        compress: true,
        proxy: {
            '/log': {
                target: 'ws://localhost:9001',
                ws: true
            },
            '/scan': {
                target: 'ws://localhost:9002',
                ws: true
            }
        },
        port: 9000,
        before: function(app, server, compiler) {
            //      var bodyParser = require('body-parser');
            //      app.use(bodyParser.json());
            // console.log(server);
            var WebSocket = require("ws");
            var wsl = new WebSocket.Server({ port: 9001 });
            wsl.on("connection", function(ws) {
                //  console.log("Connesso WS",ws,ws._socket.server);
                ws.on('message', function(msg) {
                    console.log("--> %s", msg);
                    ws.send(msg);
                });
                ws.send('hello!\n');
                setInterval(() => {
                    ws.send("[Debug] bool EvoWebserver::handleFileRead(AsyncWebServerRequest*) handleFileRead: /favicon.ico smarttemp.local [EvoWebserver.cpp:227]\n");
                }, 1000);
            });

            var wss = new WebSocket.Server({ port: 9002 });
            wss.on("connection", function(ws) {
                //  console.log("Connesso WS Scan",ws,ws._socket.server);
                //  ws.on('message', function(msg) {
                //    console.log("--> %s",msg);
                //    ws.send(msg);
                //  });
                //  ws.send('hello!');
            });

            let mode = "Auto";
            let hold = "Confort";
            let away = true

            app.get('/ostat', function(req, res) {
                res.json({
                    "otab": "home",
                    //          "bdg": "Wifi not configured"
                    "mode": mode,
                    "hold": hold,
                    "away": away,
                });
            });

            app.get('/load', function(req, res) {
                res.json({
                    "teco": 18.5,
                    "tnorm": 19.5,
                    "tconf": 20.5,
                    "aautotimeout": 30,
                    "dsleep": 35,
                    "doff": 60,
                    "tadelta": -0.5,
                    "tamode": 3,
                    "tprec": 0.2,
                    "sres": 30,
                    "nname": "SmartTemp",
                    "saddress": "hassio4.local",
                    "port": "1883",
                    "user": "mqttapi",
                    "password": "supersecret",
                    "tprefix": "",
                });
            });
            app.get('/loadW', function(req, res) {
                res.json({
                    "ssid": "MySuperNetwork",
                    "wkey": "*SuperSecret*"
                });
            });

            app.get('/scanW', async function(req, res) {
                setTimeout(() => {
                    for (let i = 0; i < Math.round(Math.random() * 5); i++) {
                        let data = JSON.stringify({
                            "ssid": "Network" + Math.round(Math.random() * 3),
                            "dBm": Math.round(Math.random() * -100),
                            "open": Boolean(Math.round(Math.random()))
                        });
                        wss.clients.forEach(function each(client) {
                            if (client.readyState === WebSocket.OPEN) {
                                client.send(data);
                            }
                        });
                    }
                    wss.clients.forEach(function each(client) {
                        if (client.readyState === WebSocket.OPEN) {
                            client.send("{\"end\":true}");
                        }
                    });
                }, 1000);
                res.json({});
            });


            app.get('/loadP', function(req, res) {
                res.json([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
            });
            //      app.get("/screen", function(req,res){
            //        res.sendFile(path.join(__dirname, 'test/screen'+(read++ % 2)+'.pbm'));
            //      });
            app.get("/screenpbm", function(req, res) {
                res.sendFile(path.join(__dirname, 'test/screenpbm' + (read++ % 2) + '.pbm'));
            });

            // exec functions
            app.get("/cmd", function(req, res) {
                if (req.query.cmd === 'mode') {
                    mode = req.query.value;
                } else if (req.query.cmd === 'hold') {
                    hold = req.query.value;
                } else if (req.query.cmd === 'away') {
                    away = req.query.value === 'true';
                }
                res.sendStatus(204);
            });

        }
    }
};

module.exports = (env, argv) => {
    //  console.log("Env:",process.env,"ArgV:",argv);
    genv = env;
    gargv = argv;

    if (argv.mode !== 'production') {
        config.devtool = 'inline-source-map';
        //    config.devtool = 'inline-source-map';    
        //    config.plugins.push( new webpack.SourceMapDevToolPlugin({}));    
    }

    if (argv.mode === 'production') {
        config.plugins.push(new CompressionPlugin({
            deleteOriginalAssets: true,
            // test: /\.js$|\.html$|\.jpe?g$|\.ico$|\.gif$|\.png$|\.svg$|\.woff$|\.ttf$|\.wav$|\.mp3$|\.eot$/i,
            // include: /\.woff2?$/
            //      exclude: /\.html$/,
        }));
    }

    return config;
};