// routes/Arenaleague/index.js

module.exports = function(app, Arenaleague, Ccuser)
{
    var apiName = 'arn';
    var tzoffset = (new Date()).getTimezoneOffset() * 60000;
    var setRank = function(userid, rank, total){
//        console.log("call setRank: " + rank);
        
    }
    var resultLeague = function (){ // in midnight reward by rank
        console.log("result league arena: ");
        Arenaleague.find({}).sort({score : 1}) // sort -1 for ascending
                                            .exec(function(err, users){
//            console.log("result league arena: " + users.length);
            if(err) console.log("err: " + err);
            for(let i = 0; i < users.length; i++){
                
//                console.log("user " + i + ": " + JSON.stringify(users[rank]));
                if(users[i].userid){
//                    console.log("userid exist " + users[i].userid);
//                    setRank(users[i].userid, i, users.length);
                    Ccuser.findOne({_id: users[i].userid}, function(err, userWithID){
                        if(err) {
                            console.log("arena result league error: " + err);
                        }
                        if(userWithID){
                            userWithID.arena_reward = i+"_"+users.length;
                            userWithID.save(function(err){
                                if(err){
                                    console.error(err);
                                }else{

                                }
                            });
                        }
                    });

//                    Ccuser.findOne({_id: users[rank].userid}, function(err, userWithID){
//                        console.log("found rank " + i + ": " + users[rank].userid);
//                        if(err) {
//                            console.log("arena result league error: " + err);
//                        }
//                        if(userWithID){
//                            userWithID.arena_reward = rank+"_"+users.length;
//                            console.log("result user: " + JSON.stringify(userWithID));
//                            console.log("arena reward: " + userWithID.arena_reward);
//                            userWithID.save(function(err){
//                                if(err){
//                                    console.error(err);
//                                }else{
//
//                                }
//                            });
//                        }
//                    });
                }
            }
            
            Arenaleague.deleteMany({}, function (err, _) {}); // delete all
        });
    }
    Arenaleague.resultLeague = resultLeague;
    
    // save user
    app.post('/cc/' + apiName + 'sv', function(req, res){
        console.log("saveuser data:" +  JSON.stringify(req.body));
        var callback = function(err, userWithCondition){
            if(err) return res.status(500).json({ error: 'database failure' });
            
            var user;
            if(userWithCondition) { // user with the name exist
                user = userWithCondition;
            }else{
                user = new Arenaleague();
                user.name = req.body.name;
                user.userid = req.body._id;
            }
            user.score = req.body.score;
            user.save(function(err){
                if(err){
                    console.error(err);
                    res.json({error: "save failed"});
                }else{
                    res.json({result: 'success', arenarid: user._id});
                }
            });
        }
        if(req.body.leaguerid){
            param = {_id: req.body.leaguerid}
        }else{
            param = {userid: req.body._id}
        }
        
        Arenaleague.findOne(param, callback);
    });
    
    // get rank
    app.post('/cc/get'+apiName+'rank', function(req,res){
//        console.log("rank req id: " + req.body.id);
        Arenaleague.find().sort({score : 1}) // sort -1 for ascending
                                            .exec(function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});

            var names = "";
            var you;
            var str;
            var user;
            for(var i = 0; i < users.length; i++){
                str = "";
                if(i < 100){
                    user = users[i];
                    str = user.name + "_" + user.score + "_" + user.userid + ",";
                }

                if(str.length > 0){
                    names += str;
                }
                if(users[i].userid == req.body._id){
                    you = i;
                }
            }
            
            res.json({names:names, you:you, total:users.length});
        });
    });
}
