/* Copyright 2017-2018 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */
#include "box2DItem.h"
#include <QDebug>
#include "src/engine/model/physics/box2DPhysicsEngine.h"
#include "src/engine/items/solidItem.h"

using namespace twoDModel::model::physics;
using namespace twoDModel::items;
using namespace parts;

Box2DItem::Box2DItem(twoDModel::model::physics::Box2DPhysicsEngine *engine
		, const SolidItem *item, const b2Vec2 &pos, float angle)
	: mIsCircle(false)
	, mEngine(*engine)
{
	qDebug() << __LINE__ << __FILE__;
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.position = pos;
	bodyDef.rotation = b2MakeRot(angle);
	if (item->bodyType() == SolidItem::DYNAMIC) {
		bodyDef.type = b2_dynamicBody;
	} else if (item->bodyType() == SolidItem::KINEMATIC) {
		bodyDef.type = b2_kinematicBody;
	} else if (item->bodyType() == SolidItem::STATIC) {
		bodyDef.type = b2_staticBody;
	}
	qDebug() << __LINE__ << __FILE__;

	auto worldId = this->mEngine.box2DWorldId();
	mBodyId = b2CreateBody(worldId, &bodyDef);
	b2Body_SetAngularDamping(mBodyId, item->angularDamping());
	b2Body_SetLinearDamping(mBodyId, item->linearDamping());
	b2ShapeDef fixtureDef = b2DefaultShapeDef();
	fixtureDef.material.restitution = 0.8f;
	qDebug() << __LINE__ << __FILE__;
	QPolygonF collidingPolygon = item->collidingPolygon();
	QPointF localCenter = collidingPolygon.boundingRect().center();
	b2Circle circleShape = {};
	b2Polygon polygonShape = {};
	qDebug() << __LINE__ << __FILE__;
	if (item->isCircle()) {
		        qDebug() << __LINE__ << __FILE__;
		mIsCircle = true;
		qreal radius = collidingPolygon.boundingRect().height() / 2;
		circleShape.radius = this->mEngine.pxToM(radius);
		fixtureDef.density = engine->computeDensity(radius, item->mass());
		        qDebug() << __LINE__ << __FILE__;
	} else {
		if (collidingPolygon.isClosed()) {
			collidingPolygon.removeLast();
		}

		mPolygon = new b2Vec2[collidingPolygon.size()];
		for (int i = 0; i < collidingPolygon.size(); ++i) {
			mPolygon[i] = engine->positionToBox2D(collidingPolygon.at(i) - localCenter);
		}

		b2Hull hull = b2ComputeHull(mPolygon, collidingPolygon.size());
		polygonShape = b2MakePolygon(&hull, 0.0f);
		fixtureDef.density = engine->computeDensity(collidingPolygon, item->mass());
	}

	        qDebug() << __LINE__ << __FILE__;
	fixtureDef.material.friction = item->friction();
	if (item->isCircle()) {
		 qDebug() << __LINE__ << __FILE__;
		b2CreateCircleShape(mBodyId, &fixtureDef, &circleShape);
		 qDebug() << __LINE__ << __FILE__;
	} else {
		b2CreatePolygonShape(mBodyId, &fixtureDef, &polygonShape);
	}
	        qDebug() << __LINE__ << __FILE__;
	b2Body_SetUserData(mBodyId, this);
}

Box2DItem::~Box2DItem()
{
	b2DestroyBody(mBodyId);
	if (!mIsCircle) {
		delete[] mPolygon;
	}
}

void Box2DItem::moveToPosition(const b2Vec2 &pos)
{
	b2Body_SetTransform(mBodyId, pos, b2Body_GetRotation(mBodyId));
	mPreviousPosition = b2Body_GetPosition(mBodyId);
}

void Box2DItem::setRotation(float angle)
{
	b2Body_SetTransform(mBodyId, b2Body_GetPosition(mBodyId), b2MakeRot(angle));
	mPreviousRotation = b2Rot_GetAngle(b2Body_GetRotation(mBodyId));
}

const b2Vec2 &Box2DItem::getPosition()
{
	mPreviousPosition = b2Body_GetPosition(mBodyId);
	return mPreviousPosition;
}

float Box2DItem::getRotation()
{
	mPreviousRotation = b2Rot_GetAngle(b2Body_GetRotation(mBodyId));
	return mPreviousRotation;
}

b2BodyId Box2DItem::getBodyId() const
{
	return mBodyId;
}

bool Box2DItem::angleOrPositionChanged() const
{
	auto angle = b2Rot_GetAngle(b2Body_GetRotation(mBodyId));
	auto position = b2Body_GetPosition(mBodyId);
	return b2Distance(mPreviousPosition, position) > FLT_EPSILON
			|| qAbs(mPreviousRotation - angle) > FLT_EPSILON;
}
