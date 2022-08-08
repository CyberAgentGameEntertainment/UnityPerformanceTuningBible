"use strict";

const expand = require("glob-expand");
const prh = require("prh");
const assert = require("assert");

const ymlList = expand({ filter: "isFile", cwd: __dirname }, [
    "**/*.yml",
    "!node_modules/**",
]);

ymlList.forEach(yml => {
    try {
        const engine = prh.fromYAMLFilePath(yml);
        const changeSet = engine.makeChangeSet("./README.ja.md");
    } catch (e) {
        console.log(`processing... ${yml}\n`);
        throw e;
    }
});

console.log("😸 done");
