[1mdiff --git a/.gitmodules b/.gitmodules[m
[1mindex 6197d4761..3efc2246e 100644[m
[1m--- a/.gitmodules[m
[1m+++ b/.gitmodules[m
[36m@@ -23,7 +23,7 @@[m
 	branch = master[m
 [submodule "plugins/robots/thirdparty/Box2D/Box2D"][m
 	path = plugins/robots/thirdparty/Box2D/Box2D[m
[31m-	url = https://github.com/iakov/Box2D[m
[32m+[m	[32murl = https://github.com/erincatto/box2d[m
 	branch = "trik-studio"[m
 [submodule "thirdparty/qslog/qslog"][m
 	path = thirdparty/qslog/qslog[m
[1mdiff --git a/installer/nxt-tools b/installer/nxt-tools[m
[1m--- a/installer/nxt-tools[m
[1m+++ b/installer/nxt-tools[m
[36m@@ -1 +1 @@[m
[31m-Subproject commit f7006a76384b8687e60587ce9d0d8ab0685f8974[m
[32m+[m[32mSubproject commit f7006a76384b8687e60587ce9d0d8ab0685f8974-dirty[m
[1mdiff --git a/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.cpp b/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.cpp[m
[1mindex 275ec8907..92c2f2bee 100644[m
[1m--- a/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.cpp[m
[1m+++ b/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.cpp[m
[36m@@ -41,10 +41,12 @@[m [mBox2DPhysicsEngine::Box2DPhysicsEngine (const WorldModel &worldModel[m
 		, const QList<RobotModel *> &robots)[m
 	: PhysicsEngineBase(worldModel, robots)[m
 	, mPixelsInCm(worldModel.pixelsInCm() * scaleCoeff)[m
[31m-	, mWorld(new b2World(b2Vec2(0, 0)))[m
[31m-	, mPrevPosition(b2Vec2(0, 0))[m
[32m+[m	[32m, mPrevPosition(b2Vec2{0, 0})[m
 	, mPrevAngle(0)[m
 {[m
[32m+[m	[32mb2WorldDef worldDef = b2DefaultWorldDef();[m
[32m+[m	[32mworldDef.gravity = b2Vec2{0, 0};[m
[32m+[m	[32mmWorldId = b2CreateWorld(&worldDef);[m
 	connect(&worldModel, &model::WorldModel::wallAdded,[m
 			this, [this](const QSharedPointer<QGraphicsItem> &i) {itemAdded(i.data());});[m
 	connect(&worldModel, &model::WorldModel::skittleAdded,[m
[36m@@ -58,6 +60,8 @@[m [mBox2DPhysicsEngine::Box2DPhysicsEngine (const WorldModel &worldModel[m
 Box2DPhysicsEngine::~Box2DPhysicsEngine(){[m
 	qDeleteAll(mBox2DRobots);[m
 	qDeleteAll(mBox2DResizableItems);[m
[32m+[m	[32mb2DestroyWorld(mWorldId);[m
[32m+[m	[32mmWorldId = b2_nullWorldId;[m
 }[m
 [m
 QVector2D Box2DPhysicsEngine::positionShift(model::RobotModel &robot) const[m
[36m@@ -66,7 +70,7 @@[m [mQVector2D Box2DPhysicsEngine::positionShift(model::RobotModel &robot) const[m
 		return QVector2D();[m
 	}[m
 [m
[31m-	return QVector2D(positionToScene(mBox2DRobots[&robot]->getBody()->GetPosition() - mPrevPosition));[m
[32m+[m	[32mreturn QVector2D(positionToScene(b2Body_GetPosition(mBox2DRobots[&robot]->getBodyId()) - mPrevPosition));[m
 }[m
 [m
 qreal Box2DPhysicsEngine::rotation(model::RobotModel &robot) const[m
[36m@@ -75,7 +79,8 @@[m [mqreal Box2DPhysicsEngine::rotation(model::RobotModel &robot) const[m
 		return 0;[m
 	}[m
 [m
[31m-	return angleToScene(mBox2DRobots[&robot]->getBody()->GetAngle() - mPrevAngle);[m
[32m+[m	[32mauto angle = b2Rot_GetAngle(b2Body_GetRotation(mBox2DRobots[&robot]->getBodyId()));[m
[32m+[m	[32mreturn angleToScene(angle - mPrevAngle);[m
 }[m
 [m
 void Box2DPhysicsEngine::onPressedReleasedSelectedItems(bool active)[m
[36m@@ -83,7 +88,11 @@[m [mvoid Box2DPhysicsEngine::onPressedReleasedSelectedItems(bool active)[m
 	for (auto *item : mScene->selectedItems()) {[m
 		Box2DItem *bItem = mBox2DDynamicItems.value(item, nullptr);[m
 		if (bItem) {[m
[31m-			bItem->getBody()->SetEnabled(active);[m
[32m+[m			[32mif (active) {[m
[32m+[m				[32mb2Body_Enable(bItem->getBodyId());[m
[32m+[m			[32m} else {[m
[32m+[m				[32mb2Body_Disable(bItem->getBodyId());[m
[32m+[m			[32m}[m
 		}[m
 	}[m
 }[m
[36m@@ -98,8 +107,9 @@[m [mvoid Box2DPhysicsEngine::addRobot(model::RobotModel * const robot)[m
 	PhysicsEngineBase::addRobot(robot);[m
 	addRobot(robot, robot->robotCenter(), robot->rotation());[m
 [m
[31m-	mPrevPosition = mBox2DRobots[robot]->getBody()->GetPosition();[m
[31m-	mPrevAngle = mBox2DRobots[robot]->getBody()->GetAngle();[m
[32m+[m	[32mauto BodyId = mBox2DRobots[robot]->getBodyId();[m
[32m+[m	[32mmPrevPosition = b2Body_GetPosition(BodyId);[m
[32m+[m	[32mmPrevAngle = b2Rot_GetAngle(b2Body_GetRotation(BodyId));[m
 [m
 	connect(robot, &model::RobotModel::positionChanged, this, [&] (const QPointF &newPos) {[m
 		onRobotStartPositionChanged(newPos, dynamic_cast<model::RobotModel *>(sender()));[m
[36m@@ -202,14 +212,14 @@[m [mvoid Box2DPhysicsEngine::onRecoverRobotPosition(const QPointF &pos)[m
 {[m
 	clearForcesAndStop();[m
 [m
[31m-	auto stop = [=](b2Body *body){[m
[31m-		body->SetAngularVelocity(0);[m
[31m-		body->SetLinearVelocity({0, 0});[m
[32m+[m	[32mauto stop = [=](b2BodyId bodyId){[m
[32m+[m		[32mb2Body_SetAngularVelocity(bodyId, 0);[m
[32m+[m		[32mb2Body_SetLinearVelocity(bodyId, {0, 0});[m
 	};[m
 [m
[31m-	stop(mBox2DRobots.first()->getBody());[m
[31m-	stop(mBox2DRobots.first()->getWheelAt(0)->getBody());[m
[31m-	stop(mBox2DRobots.first()->getWheelAt(1)->getBody());[m
[32m+[m	[32mstop(mBox2DRobots.first()->getBodyId());[m
[32m+[m	[32mstop(mBox2DRobots.first()->getWheelAt(0)->getBodyId());[m
[32m+[m	[32mstop(mBox2DRobots.first()->getWheelAt(1)->getBodyId());[m
 [m
 	onMouseReleased(pos, mBox2DRobots.keys().first()->startPositionMarker()->rotation());[m
 }[m
[36m@@ -224,16 +234,15 @@[m [mvoid Box2DPhysicsEngine::removeRobot(model::RobotModel * const robot)[m
 }[m
 [m
 void Box2DPhysicsEngine::recalculateParameters(qreal timeInterval)[m
[31m-{[m
[31m-	const int velocityIterations = 10;[m
[31m-	const int positionIterations = 6;[m
[32m+[m[32m{;[m
[32m+[m	[32mconst int subStepCount = 4;[m
 [m
 	model::RobotModel * const robot = mRobots.first();[m
 	if (!mBox2DRobots[robot]) {[m
 		return;[m
 	}[m
 [m
[31m-	b2Body *rBody = mBox2DRobots[robot]->getBody();[m
[32m+[m	[32mb2BodyId rBodyId = mBox2DRobots[robot]->getBodyId();[m
 	float secondsInterval = timeInterval / 1000.0f;[m
 [m
 	if (mBox2DRobots[robot]->isStopping()){[m
[36m@@ -244,7 +253,7 @@[m [mvoid Box2DPhysicsEngine::recalculateParameters(qreal timeInterval)[m
 		const qreal speed1 = pxToM(wheelLinearSpeed(*robot, robot->leftWheel())) / secondsInterval * sAdpt;[m
 		const qreal speed2 = pxToM(wheelLinearSpeed(*robot, robot->rightWheel())) / secondsInterval * sAdpt;[m
 [m
[31m-		if (qAbs(speed1) + qAbs(speed2) < b2_epsilon) {[m
[32m+[m		[32mif (qAbs(speed1) + qAbs(speed2) < 1) {[m
 			mBox2DRobots[robot]->stop();[m
 			mLeftWheels[robot]->stop();[m
 			mRightWheels[robot]->stop();[m
[36m@@ -255,10 +264,11 @@[m [mvoid Box2DPhysicsEngine::recalculateParameters(qreal timeInterval)[m
 		}[m
 	}[m
 [m
[31m-	mPrevPosition = rBody->GetPosition();[m
[31m-	mPrevAngle = rBody->GetAngle();[m
 [m
[31m-	mWorld->Step(secondsInterval, velocityIterations, positionIterations);[m
[32m+[m	[32mmPrevPosition = b2Body_GetPosition(rBodyId);[m
[32m+[m	[32mmPrevAngle = b2Rot_GetAngle(b2Body_GetRotation(rBodyId));[m
[32m+[m
[32m+[m	[32mb2World_Step(mWorldId, secondsInterval, subStepCount);[m
 [m
 	static volatile auto sThisFlagHelpsToAvoidClangError = true;[m
 	if ("If you want debug BOX2D, fix this expression to be false" && sThisFlagHelpsToAvoidClangError) {[m
[36m@@ -286,8 +296,12 @@[m [mvoid Box2DPhysicsEngine::recalculateParameters(qreal timeInterval)[m
 		}[m
 	}[m
 [m
[31m-	qreal angleRobot= angleToScene(mBox2DRobots[robot]->getBody()->GetAngle());[m
[31m-	QPointF posRobot = positionToScene(mBox2DRobots[robot]->getBody()->GetPosition());[m
[32m+[m	[32mauto bodyId = mBox2DRobots[robot]->getBodyId();[m
[32m+[m	[32mauto Angle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));[m
[32m+[m	[32mauto Position = b2Body_GetPosition(bodyId);[m
[32m+[m
[32m+[m	[32mqreal angleRobot= angleToScene(Angle);[m
[32m+[m	[32mQPointF posRobot = positionToScene(Position);[m
 	QGraphicsRectItem *rect1 = new QGraphicsRectItem(-25, -25, 60, 50);[m
 	QGraphicsRectItem *rect2 = new QGraphicsRectItem(-10, -6, 20, 10);[m
 	QGraphicsRectItem *rect3 = new QGraphicsRectItem(-10, -6, 20, 10);[m
[36m@@ -296,11 +310,20 @@[m [mvoid Box2DPhysicsEngine::recalculateParameters(qreal timeInterval)[m
 	rect1->setRotation(angleRobot);[m
 	rect1->setPos(posRobot);[m
 	rect2->setTransformOriginPoint(0, 0);[m
[31m-	rect2->setRotation(angleToScene(mBox2DRobots[robot]->getWheelAt(0)->getBody()->GetAngle()));[m
[31m-	rect2->setPos(positionToScene(mBox2DRobots[robot]->getWheelAt(0)->getBody()->GetPosition()));[m
[32m+[m
[32m+[m	[32mauto wheelBodyId = mBox2DRobots[robot]->getWheelAt(0)->getBodyId();[m
[32m+[m	[32mauto wheelBodyAngle = b2Rot_GetAngle(b2Body_GetRotation(wheelBodyId));[m
[32m+[m	[32mauto wheelBodyPosition = b2Body_GetPosition(wheelBodyId);[m
[32m+[m
[32m+[m	[32mauto wheel1BodyId = mBox2DRobots[robot]->getWheelAt(1)->getBodyId();[m
[32m+[m	[32mauto wheel1BodyAngle = b2Rot_GetAngle(b2Body_GetRotation(wheel1BodyId));[m
[32m+[m	[32mauto wheel1BodyPosition = b2Body_GetPosition(wheel1BodyId);[m
[32m+[m
[32m+[m	[32mrect2->setRotation(angleToScene(wheelBodyAngle));[m
[32m+[m	[32mrect2->setPos(positionToScene(wheelBodyPosition));[m
 	rect3->setTransformOriginPoint(0, 0);[m
[31m-	rect3->setRotation(angleToScene(mBox2DRobots[robot]->getWheelAt(1)->getBody()->GetAngle()));[m
[31m-	rect3->setPos(positionToScene(mBox2DRobots[robot]->getWheelAt(1)->getBody()->GetPosition()));[m
[32m+[m	[32mrect3->setRotation(angleToScene(wheel1BodyAngle));[m
[32m+[m	[32mrect3->setPos(positionToScene(wheel1BodyPosition));[m
 	mScene->addItem(rect1);[m
 	mScene->addItem(rect2);[m
 	mScene->addItem(rect3);[m
[36m@@ -319,7 +342,7 @@[m [mvoid Box2DPhysicsEngine::recalculateParameters(qreal timeInterval)[m
 [m
 	const QMap<const view::SensorItem *, Box2DItem *> sensors = mBox2DRobots[robot]->getSensors();[m
 	for (Box2DItem * sensor : sensors.values()) {[m
[31m-		const b2Vec2 position = sensor->getBody()->GetPosition();[m
[32m+[m		[32mconst b2Vec2 position = b2Body_GetPosition(sensor->getBodyId());[m
 		QPointF scenePos = positionToScene(position);[m
 		path.addEllipse(scenePos, 10, 10);[m
 	}[m
[36m@@ -348,7 +371,8 @@[m [mvoid Box2DPhysicsEngine::wakeUp()[m
 void Box2DPhysicsEngine::nextFrame()[m
 {[m
 	for(QGraphicsItem *item : mBox2DDynamicItems.keys()) {[m
[31m-		if (mBox2DDynamicItems[item]->getBody()->IsEnabled() && mBox2DDynamicItems[item]->angleOrPositionChanged()) {[m
[32m+[m		[32mauto isEnabled = b2Body_IsEnabled(mBox2DDynamicItems[item]->getBodyId());[m
[32m+[m		[32mif (isEnabled && mBox2DDynamicItems[item]->angleOrPositionChanged()) {[m
 			QPointF scenePos = positionToScene(mBox2DDynamicItems[item]->getPosition());[m
 			item->setPos(scenePos - item->boundingRect().center());[m
 			item->setRotation(angleToScene(mBox2DDynamicItems[item]->getRotation()));[m
[36m@@ -358,11 +382,11 @@[m [mvoid Box2DPhysicsEngine::nextFrame()[m
 [m
 void Box2DPhysicsEngine::clearForcesAndStop()[m
 {[m
[31m-	mWorld->ClearForces();[m
[32m+[m	[32m// LOLME: mWorld->ClearForces();[m
 	for (auto item : mBox2DDynamicItems) {[m
[31m-		b2Body *body = item->getBody();[m
[31m-		body->SetLinearVelocity({0, 0});[m
[31m-		body->SetAngularVelocity(0);[m
[32m+[m		[32mb2BodyId bodyId = item->getBodyId();[m
[32m+[m		[32mb2Body_SetLinearVelocity(bodyId, {0, 0});[m
[32m+[m		[32mb2Body_SetAngularVelocity(bodyId, 0);[m
 	}[m
 }[m
 [m
[36m@@ -449,9 +473,9 @@[m [mvoid Box2DPhysicsEngine::itemRemoved(QGraphicsItem * const item)[m
 	mBox2DDynamicItems.remove(item);[m
 }[m
 [m
[31m-b2World &Box2DPhysicsEngine::box2DWorld()[m
[32m+[m[32mb2WorldId Box2DPhysicsEngine::box2DWorldId()[m
 {[m
[31m-	return *mWorld.data();[m
[32m+[m	[32mreturn mWorldId;[m
 }[m
 [m
 float Box2DPhysicsEngine::pxToCm(qreal px) const[m
[36m@@ -461,7 +485,7 @@[m [mfloat Box2DPhysicsEngine::pxToCm(qreal px) const[m
 [m
 b2Vec2 Box2DPhysicsEngine::pxToCm(const QPointF &posInPx) const[m
 {[m
[31m-	return b2Vec2(pxToCm(posInPx.x()), pxToCm(posInPx.y()));[m
[32m+[m	[32mreturn b2Vec2{pxToCm(posInPx.x()), pxToCm(posInPx.y())};[m
 }[m
 [m
 qreal Box2DPhysicsEngine::cmToPx(float cm) const[m
[36m@@ -492,7 +516,7 @@[m [mQPointF Box2DPhysicsEngine::positionToScene(b2Vec2 boxCoords) const[m
 [m
 QPointF Box2DPhysicsEngine::positionToScene(float x, float y) const[m
 {[m
[31m-	b2Vec2 invertedCoords = b2Vec2(x, -y);[m
[32m+[m	[32mb2Vec2 invertedCoords = b2Vec2{x, -y};[m
 	return cmToPx(100.0f * invertedCoords);[m
 }[m
 [m
[1mdiff --git a/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.h b/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.h[m
[1mindex 17b132b05..85e58c6b6 100644[m
[1m--- a/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.h[m
[1m+++ b/plugins/robots/common/twoDModel/src/engine/model/physics/box2DPhysicsEngine.h[m
[36m@@ -75,7 +75,7 @@[m [mpublic:[m
 	qreal computeDensity(const QPolygonF &shape, qreal mass);[m
 	qreal computeDensity(qreal radius, qreal mass);[m
 [m
[31m-	b2World &box2DWorld();[m
[32m+[m	[32mb2WorldId box2DWorldId();[m
 [m
 public slots:[m
 	void onItemDragged(graphicsUtils::AbstractItem *item);[m
[36m@@ -97,7 +97,7 @@[m [mprivate:[m
 [m
 	twoDModel::view::TwoDModelScene *mScene {}; // Doesn't take ownership[m
 	qreal mPixelsInCm;[m
[31m-	QScopedPointer<b2World> mWorld;[m
[32m+[m	[32mb2WorldId mWorldId;[m
 [m
 	QMap<RobotModel *, parts::Box2DRobot *> mBox2DRobots;  // Takes ownership on b2Body instances[m
 	QMap<RobotModel *, parts::Box2DWheel *> mLeftWheels;  // Does not take ownership[m
[1mdiff --git a/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.cpp b/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.cpp[m
[1mindex 8070bcfcc..4c53cb8c6 100644[m
[1m--- a/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.cpp[m
[1m+++ b/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.cpp[m
[36m@@ -25,9 +25,9 @@[m [mBox2DItem::Box2DItem(twoDModel::model::physics::Box2DPhysicsEngine *engine[m
 	: mIsCircle(false)[m
 	, mEngine(*engine)[m
 {[m
[31m-	b2BodyDef bodyDef;[m
[32m+[m	[32mb2BodyDef bodyDef = b2DefaultBodyDef();[m
 	bodyDef.position = pos;[m
[31m-	bodyDef.angle = angle;[m
[32m+[m	[32mbodyDef.rotation = b2MakeRot(angle);[m
 	if (item->bodyType() == SolidItem::DYNAMIC) {[m
 		bodyDef.type = b2_dynamicBody;[m
 	} else if (item->bodyType() == SolidItem::KINEMATIC) {[m
[36m@@ -36,22 +36,25 @@[m [mBox2DItem::Box2DItem(twoDModel::model::physics::Box2DPhysicsEngine *engine[m
 		bodyDef.type = b2_staticBody;[m
 	}[m
 [m
[31m-	mBody = this->mEngine.box2DWorld().CreateBody(&bodyDef);[m
[31m-	mBody->SetAngularDamping(item->angularDamping());[m
[31m-	mBody->SetLinearDamping(item->linearDamping());[m
[32m+[m	[32mauto worldId = this->mEngine.box2DWorldId();[m
[32m+[m	[32mmBodyId = b2CreateBody(worldId, &bodyDef);[m
[32m+[m	[32mb2Body_SetAngularDamping(mBodyId, item->angularDamping());[m
[32m+[m	[32mb2Body_SetLinearDamping(mBodyId, item->linearDamping());[m
[32m+[m
[32m+[m	[32mb2ShapeDef fixtureDef = b2DefaultShapeDef();[m
[32m+[m	[32mb2SurfaceMaterial surfaceMaterial = b2DefaultSurfaceMaterial();[m
[32m+[m	[32msurfaceMaterial.restitution = 0.8;[m
 [m
[31m-	b2FixtureDef fixture;[m
[31m-	fixture.restitution = 0.8;[m
 	QPolygonF collidingPolygon = item->collidingPolygon();[m
 	QPointF localCenter = collidingPolygon.boundingRect().center();[m
[31m-	b2CircleShape circleShape;[m
[31m-	b2PolygonShape polygonShape;[m
[32m+[m	[32mb2Circle circleShape;[m
[32m+[m	[32mb2Polygon polygonShape;[m
[32m+[m
 	if (item->isCircle()) {[m
 		mIsCircle = true;[m
 		qreal radius = collidingPolygon.boundingRect().height() / 2;[m
[31m-		circleShape.m_radius = this->mEngine.pxToM(radius);[m
[31m-		fixture.shape = &circleShape;[m
[31m-		fixture.density = engine->computeDensity(radius, item->mass());[m
[32m+[m		[32mcircleShape.radius = this->mEngine.pxToM(radius);[m
[32m+[m		[32mfixtureDef.density = engine->computeDensity(radius, item->mass());[m
 	} else {[m
 		if (collidingPolygon.isClosed()) {[m
 			collidingPolygon.removeLast();[m
[36m@@ -62,19 +65,24 @@[m [mBox2DItem::Box2DItem(twoDModel::model::physics::Box2DPhysicsEngine *engine[m
 			mPolygon[i] = engine->positionToBox2D(collidingPolygon.at(i) - localCenter);[m
 		}[m
 [m
[31m-		polygonShape.Set(mPolygon, collidingPolygon.size());[m
[31m-		fixture.shape = &polygonShape;[m
[31m-		fixture.density = engine->computeDensity(collidingPolygon, item->mass());[m
[32m+[m		[32mb2Hull hull = b2ComputeHull(mPolygon, collidingPolygon.size());[m
[32m+[m		[32mpolygonShape = b2MakePolygon(&hull, 0.0f);[m
[32m+[m		[32mfixtureDef.density = engine->computeDensity(collidingPolygon, item->mass());[m
 	}[m
 [m
[31m-	fixture.friction = item->friction();[m
[31m-	mBody->CreateFixture(&fixture);[m
[31m-	mBody->SetUserData(this);[m
[32m+[m	[32msurfaceMaterial.friction = item->friction();[m
[32m+[m	[32mfixtureDef.material = surfaceMaterial;[m
[32m+[m	[32mif (item->isCircle()) {[m
[32m+[m		[32mb2CreateCircleShape(mBodyId, &fixtureDef, &circleShape);[m
[32m+[m	[32m} else {[m
[32m+[m		[32mb2CreatePolygonShape(mBodyId, &fixtureDef, &polygonShape);[m
[32m+[m	[32m}[m
[32m+[m	[32mb2Body_SetUserData(mBodyId, this);[m
 }[m
 [m
 Box2DItem::~Box2DItem()[m
 {[m
[31m-	mBody->GetWorld()->DestroyBody(mBody);[m
[32m+[m	[32mb2DestroyBody(mBodyId);[m
 	if (!mIsCircle) {[m
 		delete[] mPolygon;[m
 	}[m
[36m@@ -82,35 +90,37 @@[m [mBox2DItem::~Box2DItem()[m
 [m
 void Box2DItem::moveToPosition(const b2Vec2 &pos)[m
 {[m
[31m-	mBody->SetTransform(pos, mBody->GetAngle());[m
[31m-	mPreviousPosition = mBody->GetPosition();[m
[32m+[m	[32mb2Body_SetTransform(mBodyId, pos, b2Body_GetRotation(mBodyId));[m
[32m+[m	[32mmPreviousPosition = b2Body_GetPosition(mBodyId);[m
 }[m
 [m
 void Box2DItem::setRotation(float angle)[m
 {[m
[31m-	mBody->SetTransform(mBody->GetPosition(), angle);[m
[31m-	mPreviousRotation = mBody->GetAngle();[m
[32m+[m	[32mb2Body_SetTransform(mBodyId, b2Body_GetPosition(mBodyId), b2MakeRot(angle));[m
[32m+[m	[32mmPreviousRotation = b2Rot_GetAngle(b2Body_GetRotation(mBodyId));[m
 }[m
 [m
 const b2Vec2 &Box2DItem::getPosition()[m
 {[m
[31m-	mPreviousPosition = mBody->GetPosition();[m
[32m+[m	[32mmPreviousPosition = b2Body_GetPosition(mBodyId);[m
 	return mPreviousPosition;[m
 }[m
 [m
 float Box2DItem::getRotation()[m
 {[m
[31m-	mPreviousRotation = mBody->GetAngle();[m
[32m+[m	[32mmPreviousRotation = b2Rot_GetAngle(b2Body_GetRotation(mBodyId));[m
 	return mPreviousRotation;[m
 }[m
 [m
[31m-b2Body *Box2DItem::getBody() const[m
[32m+[m[32mb2BodyId Box2DItem::getBodyId() const[m
 {[m
[31m-	return mBody;[m
[32m+[m	[32mreturn mBodyId;[m
 }[m
 [m
 bool Box2DItem::angleOrPositionChanged() const[m
 {[m
[31m-	return b2Distance(mPreviousPosition, mBody->GetPosition()) > b2_epsilon[m
[31m-			|| qAbs(mPreviousRotation - mBody->GetAngle()) > b2_epsilon;[m
[32m+[m	[32mauto angle = b2Rot_GetAngle(b2Body_GetRotation(mBodyId));[m
[32m+[m	[32mauto position = b2Body_GetPosition(mBodyId);[m
[32m+[m	[32mreturn b2Distance(mPreviousPosition, position) > 1[m
[32m+[m			[32m|| qAbs(mPreviousRotation - angle) > 1;[m
 }[m
[1mdiff --git a/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.h b/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.h[m
[1mindex 41612ee0f..a0dcf1a98 100644[m
[1m--- a/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.h[m
[1m+++ b/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DItem.h[m
[36m@@ -43,14 +43,13 @@[m [mpublic:[m
 [m
 	const b2Vec2 &getPosition();[m
 	float getRotation();[m
[31m-[m
[31m-	b2Body *getBody() const;[m
[32m+[m	[32mb2BodyId getBodyId() const;[m
 [m
 	/// Returns true if the position or angle were changed since previous get or set calls.[m
 	bool angleOrPositionChanged() const;[m
 [m
 private:[m
[31m-	b2Body *mBody; // Takes ownership[m
[32m+[m	[32mb2BodyId mBodyId; // Takes ownership[m
 	b2Vec2 *mPolygon; // Takes ownership[m
 [m
 	bool mIsCircle;[m
[1mdiff --git a/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DRobot.cpp b/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DRobot.cpp[m
[1mindex 4ea0d0930..9b81dbe2b 100644[m
[1m--- a/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DRobot.cpp[m
[1m+++ b/plugins/robots/common/twoDModel/src/engine/model/physics/parts/box2DRobot.cpp[m
[36m@@ -28,59 +28,67 @@[m [mBox2DRobot::Box2DRobot(Box2DPhysicsEngine *engine, twoDModel::model::RobotModel[m
 		, b2Vec2 pos, float angle)[m
 	: mModel(robotModel)[m
 	, mEngine(engine)[m
[31m-	, mWorld(engine->box2DWorld())[m
[32m+[m	[32m, mWorldId(engine->box2DWorldId())[m
 {[m
 	b2BodyDef bodyDef;[m
[31m-	bodyDef.position = pos;[m
[31m-	bodyDef.angle = angle;[m
 	bodyDef.type = b2_dynamicBody;[m
[31m-	mBody = mWorld.CreateBody(&bodyDef);[m
[32m+[m	[32mbodyDef.position = pos;[m
[32m+[m	[32mbodyDef.rotation = b2MakeRot(angle);[m
[32m+[m	[32mmBodyId = b2CreateBody(engine->box2DWorldId(), &bodyDef);[m
[32m+[m
[32m+[m	[32mb2ShapeDef fixtureDef = b2DefaultShapeDef();[m
[32m+[m	[32mb2SurfaceMaterial surfaceMaterial = b2DefaultSurfaceMaterial();[m
[32m+[m	[32msurfaceMaterial.restitution = 0.6;[m
[32m+[m	[32msurfaceMaterial.friction = mModel->info().friction();[m
[32m+[m	[32mfixtureDef.material = surfaceMaterial;[m
 [m
[31m-	b2FixtureDef robotFixture;[m
[31m-	b2PolygonShape polygonShape;[m
 	QPolygonF collidingPolygon = mModel->info().collidingPolygon();[m
 	QPointF localCenter = collidingPolygon.boundingRect().center();[m
[32m+[m
 	mPolygon.reset(new b2Vec2[collidingPolygon.size()]);[m
 	for (int i = 0; i < collidingPolygon.size(); ++i) {[m
 		mPolygon[i] = engine->positionToBox2D(collidingPolygon.at(i) - localCenter);[m
 	}[m
 [m
[31m-	polygonShape.Set(mPolygon.get(), collidingPolygon.size());[m
[31m-	robotFixture.shape = &polygonShape;[m
[31m-	robotFixture.density = engine->computeDensity(collidingPolygon, mModel->info().mass());[m
[31m-	robotFixture.friction = mModel->info().friction();[m
[31m-	robotFixture.restitution = 0.6;[m
[32m+[m	[32mfixtureDef.density = engine->computeDensity(collidingPolygon, mModel->info().mass());[m
 [m
[31m-	mBody->CreateFixture(&robotFixture);[m
[31m-	mBody->SetUserData(this);[m
[31m-	mBody->SetAngularDamping(1.0f);[m
[31m-	mBody->SetLinearDamping(1.0f);[m
[32m+[m	[32mb2Hull hull = b2ComputeHull(mPolygon.get(), collidingPolygon.size());[m
[32m+[m	[32mb2Polygon polygon = b2MakePolygon(&hull, 0.0f);[m
[32m+[m	[32mb2ShapeId polygonShapeId = b2CreatePolygonShape(mBodyId, &fixtureDef, &polygon);[m
[32m+[m	[32mb2Body_SetUserData(mBodyId, this);[m
[32m+[m	[32mb2Body_SetAngularDamping(mBodyId, 1.0f);[m
[32m+[m	[32mb2Body_SetLinearDamping(mBodyId, 1.0f);[m
 	connectWheels();[m
 [m
[31m-	for (int i = 0; i < polygonShape.m_count; ++i) {[m
[31m-		mDebuggingDrawPolygon.append(engine->positionToScene(polygonShape.m_vertices[i] + mBody->GetPosition()));[m
[32m+[m	[32mauto finalPolygon = b2Shape_GetPolygon(polygonShapeId);[m
[32m+[m
[32m+[m	[32mfor (int i = 0; i < finalPolygon.count; ++i) {[m
[32m+[m		[32mauto position = b2Body_GetPosition(mBodyId);[m
[32m+[m		[32mmDebuggingDrawPolygon.append(engine->positionToScene(finalPolygon.vertices[i] + position));[m
 	}[m
 [m
 	if (!mDebuggingDrawPolygon.isEmpty() && !mDebuggingDrawPolygon.isClosed()) {[m
 		mDebuggingDrawPolygon.append(mDebuggingDrawPolygon.first());[m
[31m-	}[m
[32m+[m	[32m};[m
 }[m
 [m
 Box2DRobot::~Box2DRobot() {[m
[31m-	for (auto i = mBody->GetJointList(); i; i = i->next) {[m
[31m-		mWorld.DestroyJoint(i->joint);[m
[32m+[m	[32mauto jointCapacity = b2Body_GetJointCount(mBodyId);[m
[32m+[m	[32mstd::vector<b2JointId> joints(jointCapacity);[m
[32m+[m	[32mauto jointCount = b2Body_GetJoints(mBodyId, joi