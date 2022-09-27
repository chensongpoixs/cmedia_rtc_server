Babel的功能
语法转换，将ES6+的语法转换为向后兼容的JavaScript语法
通过Polyfill方式在目标环境中添加缺失的特性（通过@babel/polyfill模块）
可转换JSX语法

Babel的安装
安装NodeJs
自行安装
安装cnpm/yarn
window+R打开命令行，输入以下命令
npm install -g cnpm --registry=https://registry.npm.taobao.org
1
输入cnpm -v检查是否正常，正常情况下会显示版本号

cnpm -v
1
安装babel
cnpm install -g @babel/core @babel/cli
1
Babel的使用
将箭头函数转换成普通函数
let getYear = () => new Date().getFullYear()
如图：
过程实现：
实际开发中，建议在当前项目中也安装一下@babel/core @babel/cli，安装的时候可以输入@babel/preset-env来添加预设的转换规则
VS终端输入以下命令：
cnpm install @babel/core @babel/cli @babel/preset-env
1
安装完后VS左侧文件栏会多出个node_modules的文件夹。现在开始转换，命令行输入以下命令：

babel test_arrow_function.js -o test.js --presets=@babel/preset-env
1
其中test_arrow_function.js是需要被转换的ES6+语法文件，test.js是转换得到的JavaScript语法文件，效果如图：
 
 
/////////////////////

```
npm install -g babel-cli

 npm install --save-dev babel-preset-es2015 babel-cli
 
```