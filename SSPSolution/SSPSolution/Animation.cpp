#include "Animation.h"

Animation::Animation()
{
	int animIndex = 0;
	this->elapsedTime = 0.0f;
	this->currentAnimation = IDLE_STATE;

	modelPtr = new Resources::Model();

	Resources::ResourceHandler::GetInstance()->GetModel(UINT(1337), modelPtr);

	skeletonPtr = modelPtr->GetSkeleton();

	jointList = skeletonPtr->GetSkeletonData()->joints;
	jointCount = skeletonPtr->GetSkeletonData()->jointCount;

	for (int i = 0; i < jointCount; i++)
	{
		ConvertFloatArrayToXMFloatMatrix(jointList[i].invBindPose);
	}
	animationPtr = skeletonPtr->GetAnimation(currentAnimation);

	animatedJointsList = animationPtr->GetAllJoints();

	int startFrame = animatedJointsList->keyframes[0].timeValue;
	int endFrame = animatedJointsList->keyframes[animatedJointsList->keyframeCount - 1].timeValue;

	/*Initialize the stack with a default "IDLE" animation.*/
	Push(currentAnimation, true, startFrame, endFrame, 0);
}

Animation::~Animation()
{
}

void Animation::Update(float dt)
{
	/*Need to check in this function what the time in the animation is.
	If the last frame have been reached, we should check if the time is
	at the end of the animation and if it's a looping animation.*/

	bool newAnimation = false;
	
	/*Check if there is a current animation in the stack.*/
	if (!animationStack.empty() || currentAnimation == IDLE_STATE)
	{
		/*Increment elapsedTime with the delta-time. Consider to multiply framerate here?*/
		elapsedTime += dt;

		/*If the animation reaches the last frame and the animation is looping, reset elapsedTime to ZERO.*/
		if (elapsedTime >= animationStack.top().endFrame && animationStack.top().isLooping == true)
		{
			elapsedTime = 0.0f;
		}

		/*If the animation reaches the last frame and the animation is NOT looping, */
		else if (elapsedTime >= animationStack.top().endFrame && animationStack.top().isLooping == false)
		{
			/*Send the elapsed time from previous animation clip, it's index and the index to the next animation.*/
			/*Blend between animations.*/
			//Blend(elapsedTime, currentAnimation, 0); //<---- Somehow get the new animation state as input argument here. 

			elapsedTime = 0.0f;

			/*Remove the previous played animation clip from stack.*/
			Pop();

			/*The animation is new and not the default state.*/
			if (newAnimation != IDLE_STATE)
			{
				//GetAnimationData(currentAnimation, isLooping, startFrame, endFrame, duration);

				/*Push the new animation to the stack.*/
				Push(0, false, 0, 0, 0);
			}

			/*The animation is the default state.*/
			else
			{
				Push(IDLE_STATE, true, 0, 0, 0);
			}
		}

		else if (elapsedTime > animationStack.top().startFrame || elapsedTime < animationStack.top().endFrame && newAnimation == true)
		{
			Push(0, false, 0, 0, 0);

			//Blend(elapsedTime, currentAnimation, 0);
		}

		/*Call this function and check if the elapsedTime is at the start, between frames or at the end.*/
		Interpolate(elapsedTime, interpolatedTransforms);
	}

	/*No current animation, the physics are currently controlling the "animatible entity".*/
	if(currentAnimation == PHYSICS_STATE)
	{
		/*Have some kind of function here that pauses the playing of any animation, while physics do the job.*/
	}
}

void Animation::Interpolate(float currentTime, std::vector<XMFLOAT4X4> interpolatedTransforms)
{
	interpolatedTransforms.resize(jointCount);

	for (int jointIndex = 0; jointIndex < jointCount; jointIndex++)
	{
		Resources::Animation::AnimationJoint animatedJoint = animatedJointsList[jointIndex];

		/*The current time is the first keyframe.*/
		if (currentTime <= animationStack.top().startFrame)
		{
			int startFrame = animationStack.top().startFrame;

			XMFLOAT3 tempTrans(animatedJoint.keyframes[startFrame].translation);
			XMFLOAT3 tempScale(animatedJoint.keyframes[startFrame].scale);
			XMFLOAT4 tempQuat(animatedJoint.keyframes[startFrame].quaternion);

			XMVECTOR trans = XMLoadFloat3(&tempTrans);
			XMVECTOR scale = XMLoadFloat3(&tempScale);
			XMVECTOR quat = XMLoadFloat4(&tempQuat);

			XMVECTOR zero = XMVectorSet(0.f, 0.f, 0.f, 0.f);

			XMStoreFloat4x4(&interpolatedTransforms[jointIndex], XMMatrixAffineTransformation(scale, zero, quat, trans));
		}
		/*The current time is the last keyframe.*/
		else if (currentTime >= animationStack.top().endFrame)
		{
			int endFrame = animationStack.top().endFrame;

			XMVECTOR trans, scale, quat;

			XMFLOAT3 tempTrans(animatedJoint.keyframes[endFrame].translation);
			XMFLOAT3 tempScale(animatedJoint.keyframes[endFrame].scale);
			XMFLOAT4 tempQuat(animatedJoint.keyframes[endFrame].quaternion);

			XMVECTOR trans = XMLoadFloat3(&tempTrans);
			XMVECTOR scale = XMLoadFloat3(&tempScale);
			XMVECTOR quat = XMLoadFloat4(&tempQuat);

			XMVECTOR zero = XMVectorSet(0.f, 0.f, 0.f, 0.f);

			XMStoreFloat4x4(&interpolatedTransforms[jointIndex], XMMatrixAffineTransformation(scale, zero, quat, trans));
		}

		else
		{
			int keyFrameCount = animatedJoint.keyframeCount;

			for (int i = 0; i < keyFrameCount; i++)
			{
				if (currentTime >= animatedJoint.keyframes[i].timeValue && currentTime <= animatedJoint.keyframes[i + 1].timeValue)
				{
					float lerpFactor = 0;

					XMFLOAT3 tempTrans1(animatedJoint.keyframes[i].translation);
					XMFLOAT3 tempTrans2(animatedJoint.keyframes[i + 1].translation);
					XMFLOAT3 tempScale1(animatedJoint.keyframes[i].scale);
					XMFLOAT3 tempScale2(animatedJoint.keyframes[i + 1].scale);
					XMFLOAT4 tempQuat1(animatedJoint.keyframes[i].quaternion);
					XMFLOAT4 tempQuat2(animatedJoint.keyframes[i + 1].quaternion);

					XMVECTOR trans1 = XMLoadFloat3(&tempTrans1);
					XMVECTOR scale1 = XMLoadFloat3(&tempScale1);
					XMVECTOR quat1 = XMLoadFloat4(&tempQuat1);

					XMVECTOR trans2 = XMLoadFloat3(&tempTrans2);
					XMVECTOR scale2 = XMLoadFloat3(&tempScale2);
					XMVECTOR quat2 = XMLoadFloat4(&tempQuat2);

					/*Zero vector for the affine transformation matrix.*/
					XMVECTOR zero = XMVectorSet(0, 0, 0, 0);

					/*Update the transform for each joint in the skeleton.*/
					/*XMStoreFloat4x4(&interpolatedTransforms[jointIndex],
						XMMatrixAffineTransformation(lerpedScale, zero, lerpedQuat, lerpedTrans));*/
				}
			}
		}
	}
}

void Animation::ConvertFloatArrayToXMFloatMatrix(float floatArray[16])
{
	XMFLOAT4X4 matrix = XMFLOAT4X4(floatArray);

	invBindPoses.push_back(matrix);
}
void Animation::CalculateFinalTransform(std::vector<XMFLOAT4X4> interpolatedTransforms)
{

}

void Animation::Blend(int lastFrame, int prevState, int newState)
{
}

void Animation::Push(int currentAnimation, bool isLooping, int startFrame, int endFrame, int duration)
{
	this->currentAnimation = currentAnimation;

	AnimationClip clip;
	clip.isLooping = isLooping;
	clip.startFrame = startFrame;
	clip.endFrame = endFrame;

	animationStack.push(clip);
}

void Animation::Pop()
{
	if (!animationStack.empty())
	{
		animationStack.pop();
	}
}

