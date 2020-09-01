module.exports= {
  timeout: parseInt(process.env.GEN_TIME_LIMIT || 4000)*2
};
console.log("Config file loaded.");
console.log(module.exports);

