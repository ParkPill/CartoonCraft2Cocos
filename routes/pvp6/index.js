// routes/Pvp6league/index.js

module.exports = function(app, Pvp6league, Ccuser)
{
    var apiName = 'p6';
    var tzoffset = (new Date()).getTimezoneOffset() * 60000;
    var setRank = function(userid, rank, trophy, total){
        
                
    }
    var resultLeague = function (){ // in midnight reward by rank
        Pvp6league.find({}).sort({trophy : -1}) // sort -1 for ascending
                                            .exec(function(err, users){
            console.log("result league pvp6");
            if(err) console.log("err: " + err);
            var toRemoveCount = users.length - 100;
            for(let i = 0; i < users.length; i++){
//                setRank(users[i].userid, i, users[i].trophy, users.length);
                let trophy = users[i].trophy;
                let total = users.length;
                Ccuser.findOne({_id: users[i].userid}, function(err, userWithID){
                    if(err) {
                        
                    }else{
                        if(userWithID){
                            userWithID.pvp_rwd_6 = i+"_"+total+"_"+trophy;
//                            console.error("user id: " + userWithID.id + " reward: " + userWithID.pvp_rwd_6); // test 
                            userWithID.save(function(err){
                                if(err){
                                    console.error("reward save err: " + err);
                                }else{
                                }
                            });
                        }
                    }
                    
                });
                if(toRemoveCount > 0){
                    toRemoveCount--;
                    Pvp6league.deleteOne({ _id: users[i]._id }, function (err) {
                        if (err) {
                            console.log("err: " + err);
                        }
//                        console.log("delete done");
                        // deleted at most one tank document
                    });
                }else{
                    users[i].trophy = 1000;
                    users[i].save(function(err){
                        if(err){
                            console.error("pvp user save: " + err);
                        }else{

                        }
                    });
                }
            }
            
//            Pvp6league.deleteMany({}, function (err, _) {}); // delete all test 
        });
    }
    Pvp6league.resultLeague = resultLeague;
    
    // save user
    app.post('/cc/' + apiName + 'sv', function(req, res){
//        console.log("saveuser data:" +  JSON.stringify(req.body));
        var callback = function(err, userWithCondition){
            if(err) return res.status(500).json({ error: 'database failure' });
            
            var user;
            if(userWithCondition) { // user with the name exist
                user = userWithCondition;
            }else{
                user = new Pvp6league();
                user.name = req.body.name;
                user.userid = req.body._id;
            }
            
            user.trophy = req.body.trophy;
            user.data = req.body.data;
            user.save(function(err){
                if(err){
                    console.error(err);
                    res.json({error: "save failed"});
                }else{
                    res.json({result: 'success'});
                }
            });
        }
        param = {userid: req.body._id}
        Pvp6league.findOne(param, callback);
    });
    
    // get rank
    app.post('/cc/get'+apiName+'rank', function(req,res){
//        console.log("rank req id: " + req.body.id);
        Pvp6league.find().sort({trophy : -1}) // sort -1 for ascending
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
                    str = user.name + "|" + user.trophy + "|" + user.data + ",";
                }
//                console.log(i + ". " + str);
                if(str.length > 0){
                    names += str;
                }
                if(users[i].userid == req.body._id){
                    you = i + "|" + users[i].trophy;
                }
            }
            
            res.json({names:names, you:you, total:users.length});
        });
    });
    
    // find match
    app.post('/cc/' + apiName + 'findmatch', function(req,res){
        var min = req.body.trophy - 500;
        Pvp6league.find({trophy:{$gt:min}}
                    , function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                var newUser = new Pvp6league();
                newUser.name = "Cartoony Hero";
                newUser.userid = "0";
                newUser.trophy = 1240;
                newUser.data = "_60/101/8/9_61/101/9/9_62/101/10/9_63/101/11/9_52/100/12/9_53/100/13/9_57/100/14/9_57/100/15/9_55/100/15/6_50/0/17/9_64/101/18/9_54/100/19/9_";
                users.push(newUser);
            }
            var data = {};
            var user = users[Math.floor((Math.random() * users.length))];
//            console.log("found pvp6 user for match: " + user.userid);
            data.userid = user.userid;
            data.data = user.data;
            data.name = user.name;
            data.trophy = user.trophy;
//            console.log("match user: " + JSON.stringify(data));
            res.json(data);
        });
    });
    
}
